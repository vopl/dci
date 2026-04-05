/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "connector.hpp"

namespace dci::module::ppn::transport
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::Connector()
        : api::Connector<>::Opposite(idl::interface::Initializer())
    {

        //in connect(Address) -> Channel;
        methods()->connect() += serviceSol() * [this](api::Address&& address)
        {
            /*
             * останов:
             *      - по разрушении хаба
             *      - по отмене результата
             * старт:
             *      - по готовности конечных точек
             */

            return cmt::spawnv() += _tol * [address=std::move(address), this](cmt::Promise<api::Channel<>>& out)
            {
                cmt::task::Face tf = cmt::task::current();
                tf.stopOnResolvedCancel(out);//остановить этот воркер по отмене результата

                Set<api::connector::Downstream<>> probes;
                List<cmt::Future<api::Channel<>>> results;
                sbs::Owner probesOwner;

                while(!out.resolved())
                {
                    api::connector::Downstream<> probe = getBestDownstreamFor(address, probes);
                    if(!probe)
                    {
                        break;
                    }

                    probes.insert(probe);

                    cmt::Future<api::Channel<>> result = probe->connect(address);
                    result.then() += probesOwner * [&out, tf](auto in) mutable
                    {
                        if(in.resolvedValue() && !out.resolved())
                        {
                            out.resolveValue(in.detachValue());
                            tf.stop(false);

                            return;
                        }
                    };

                    if(out.resolved())
                    {
                        return;
                    }

                    results.emplace_back(result);

                    poll::WaitableTimer tmr{std::chrono::seconds{2}};
                    tmr.start();
                    tmr.wait();
                }

                cmt::waitAll(results);
                probesOwner.flush();

                if(!out.resolved())
                {
                    String lle;
                    for(cmt::Future<api::Channel<>>& result : results)
                    {
                        if(result.resolvedException())
                        {
                            lle += " (subcause: "+exception::toString(result.detachException())+")";
                        }
                        else if(result.resolvedCancel())
                        {
                            lle += " (subcause: cancel)";
                        }
                    }

                    api::connector::ConnectionRefused err{"all lower layers failed for "+address.value+lle};
                    out.resolveException(exception::buildInstance<api::connector::ConnectionRefused>(err));
                }

            };
        };

        //in add(Connector);
        methods()->add() += serviceSol() * [this](api::connector::Downstream<>&& instance)
        {
            _endpoints.add(std::move(instance));
        };

        //in del(Connector);
        methods()->del() += serviceSol() * [this](const api::connector::Downstream<>& instance)
        {
            _endpoints.del(instance);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::~Connector()
    {
        serviceSol().flush();
        _endpoints.clear();
        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        double weightCost(real64 cost)
        {
            return std::min(std::max(1.0 - cost, 1.0), 0.0);
        }

        double weightRtt(real64 rtt)
        {
            if(rtt <= 0)
            {
                return 1.0;
            }

            return std::min(std::max(1.0/rtt, 1.0), 0.0);
        }

        double weightBandwidth(real64 bandwidth)
        {
            return std::min(std::max(bandwidth/1024/1024, 1.0), 0.0);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::connector::Downstream<> Connector::getBestDownstreamFor(const api::Address& address, const Set<api::connector::Downstream<>>& blacklist)
    {
        auto scheme = utils::uri::scheme(address.value);
        utils::ip::Scope scope = scheme.starts_with("tcp") ?
                                         utils::ip::scope(utils::uri::hostPort(address.value)) :
                                         utils::ip::Scope::null;

        for(;;)
        {
            if(_endpoints.empty())
            {
                break;
            }

            List<cmt::Event*> unreadies;
            double maxWeight = -1;
            api::connector::Downstream<> candidate;

            for(auto& endpoint : _endpoints)
            {
                if(blacklist.count(endpoint->_instance))
                {
                    continue;
                }

                if(!endpoint->_ready.isRaised())
                {
                    unreadies.push_back(&endpoint->_ready);
                    continue;
                }

                if(utils::uri::scheme(endpoint->_address.value) != scheme)
                {
                    continue;
                }

                if(scheme.starts_with("tcp"))
                {
                    //слабо... Тут по хорошему надо бы анализировать таблицу маршрутизации...
                    bool bad = false;
                    switch(endpoint->_addressIpScope)
                    {
                    case utils::ip::Scope::host4:
                        bad = utils::ip::Scope::host4 != scope;
                        break;
                    case utils::ip::Scope::host6:
                        bad = utils::ip::Scope::host6 != scope;
                        break;
                    default:
                        break;
                    }

                    if(bad)
                    {
                        continue;
                    }
                }

                double weight = weightCost(endpoint->_cost) + weightRtt(endpoint->_rtt) + weightBandwidth(endpoint->_bandwidth);

                if(weight > maxWeight)
                {
                    maxWeight = weight;
                    candidate = endpoint->_instance;
                }
            }

            if(candidate)
            {
                return candidate;
            }

            if(!unreadies.empty())
            {
                cmt::waitAny(unreadies);
                continue;
            }

            break;
        }

        return api::connector::Downstream<>{};
    }
}
