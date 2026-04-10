/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "slave.hpp"
#include "slave/factory.hpp"

namespace dci::module::ppn::service
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Slave::Slave(host::Manager* hostManager)
        : idl::gen::ppn::service::Slave<>::Opposite{idl::interface::Initializer{}}
        , _hostManager{hostManager}
    {
        {
            link::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](link::feature::Service<> srv)
            {
                srv->addPayload(*this);
            };
        }

        {
            link::feature::Payload<>::Opposite op = *this;

            //in ids() -> set<ilid>;
            op->ids() += serviceSol() * []()
            {
                return cmt::readyFuture(Set<idl::ILid>{api::Factory<>::lid()});
            };

            //in getInstance(Id requestorId, Remote requestor, ilid) -> interface;
            op->getInstance() += serviceSol() * [this](const link::Id& masterId, const link::Remote<>&, idl::ILid ilid)
            {
                auto iter = _master2Rules.find(masterId);
                if(_master2Rules.end() == iter)
                {
                    return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::error::Forbidden>());
                }

                if(api::Factory<>::lid() == ilid)
                {
                    slave::Factory* factory = new slave::Factory{_hostManager, iter->second};
                    factory->involvedChanged() += factory->serviceSol() * [factory](bool v)
                    {
                        if(!v)
                        {
                            delete factory;
                        }
                    };

                    return cmt::readyFuture(idl::Interface{factory->opposite()});
                }

                dbgWarn("crazy link?");

                return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::Error>("bad instance ilid requested"));
            };
        }

        {
            idl::gen::Configurable<>::Opposite op = *this;

            op->configure() += serviceSol() * [this](dci::idl::gen::Config&& config)
            {
                auto c = config::cnvt(std::move(config));

                using namespace std::literals;
                for(const auto& [k, v] : c)
                {
                    if("master"sv == k)
                    {
                        const std::string& idStr = v.data();
                        link::Id masterId;
                        if(idStr.size() != masterId.size()*2)
                        {
                            LOGW("wrong ppn::service::Slave master id size: " << idStr);
                            continue;
                        }
                        if(!utils::h2b(idStr.c_str(), idStr.size(), masterId.data()))
                        {
                            LOGW("wrong ppn::service::Slave master id format: " << idStr);
                            continue;
                        }

                        slave::Rules& rules = _master2Rules[masterId];
                        slave::Rule& rule = rules.emplace_back();

                        for(const auto& [mode, value] : v)
                        {
                            if("allow"sv == mode)
                            {
                                rule._allow = true;
                            }
                            else if("deny"sv == mode)
                            {
                                rule._deny = false;
                            }
                            else
                            {
                                LOGW("wrong ppn::service::Slave master config record mode: " << mode);
                                continue;
                            }

                            if("*"sv == value.data())
                            {
                                rule._all = true;
                            }
                            else if(std::optional<idl::ILid> ilidOpt{_hostManager->resolveAlias(value.data())}; ilidOpt && *ilidOpt)
                            {
                                rule._concrete.emplace(ilidOpt.value());
                            }
                            else
                            {
                                LOGW("wrong ppn::service::Slave master config record value: " << value.data());
                                continue;
                            }
                        }
                    }
                    else
                    {
                        LOGW("unknown ppn::service::Slave config key: " << k);
                        continue;
                    }
                }

                return cmt::readyFuture(None{});
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Slave::~Slave()
    {
        serviceSol().flush();
    }
}
