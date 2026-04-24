/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "consumer.hpp"

namespace dci::module::ppn::service::aup::doer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Consumer::Consumer()
    {
        instance::notifiers::onBufferCatalogIncomplete() += _sol * [this](const Oid& oid)
        {
            addIncomplete(oid, Destiny::catalog);
        };

        instance::notifiers::onBufferCatalogComplete() += _sol * [this](const Oid& oid)
        {
            fixComplete(oid, Destiny::catalog);
        };

        instance::notifiers::onBufferStorageIncomplete() += _sol * [this](const Oid& oid)
        {
            addIncomplete(oid, Destiny::storage);
        };

        instance::notifiers::onBufferStorageComplete() += _sol * [this](const Oid& oid)
        {
            fixComplete(oid, Destiny::storage);
        };

        for(const Oid& oid : instance::io::bufferCatalogIncomplete())
            addIncomplete(oid, Destiny::catalog);

        for(const Oid& oid : instance::io::bufferStorageIncomplete())
            addIncomplete(oid, Destiny::storage);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Consumer::~Consumer()
    {
        _sol.flush();

        while(!_demandsProcessing.empty())
        {
            const Demand& demand{*_demandsProcessing.begin()};
            demand._delayedTransfersEmpty.clear();
            demand._delayedTransfersRevived.clear();
            demand._tol.stop();
        }

        _demands.clear();
    }

    namespace
    {
        template <class... Args>
        class logname
        {
        public:
            logname(const Args&... args)
                : _args{args...}
            {}

            friend std::ostream& operator<<(std::ostream& out, const logname& ln)
            {
                utils::overloaded f
                {
                    [&](const char* csz)
                    {
                        out << csz;
                    },
                    [&](const Array<uint8, 32>& id)
                    {
                        out << utils::b2h(id.data(), 5);
                    },
                    [&](const std::size_t& num)
                    {
                        out << num;
                    },
                    [&](const consumer::Destiny& d)
                    {
                        switch(d)
                        {
                        case consumer::Destiny::none:
                            out << "none";
                            break;
                        case consumer::Destiny::catalog:
                            out << "catalog";
                            break;
                        case consumer::Destiny::storage:
                            out << "storage";
                            break;
                        case consumer::Destiny::both:
                            out << "both";
                            break;
                        }
                    }
                };

                std::apply([&](const auto& first, const auto&... tail)
                {
                    f(first);
                    (((out << " "),f(tail)),...);
                } , ln._args);
                return out;
            }

        private:
            std::tuple<const Args& ...> _args;
        };
        template<class... Args> logname(Args...) -> logname<Args...>;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::joined(const link::Id& rid, api::Supplier<>&& api)
    {
        auto [iter, emplaced] = _suppliers.emplace(rid, _supplierNumberGen, std::move(api));
        if(!emplaced)
        {
            LOGD(logname{"rid", rid} << " secondary ignored");
            return;
        }
        ++_supplierNumberGen;

        const Supplier& supplier{*iter};
        LOGD(logname{"rid", supplier._rid, supplier._number} << " joined");

        supplier._api.involvedChanged() += supplier._sol * [this, &supplier](bool b)
        {
            if(!b)
            {
                LOGD(logname{"rid", supplier._rid, supplier._number} << " disjoined");
                _suppliers.erase(supplier._rid);
            }
        };

        supplier._api->newRelease() += supplier._sol * [this, &supplier](const Oid& oid)
        {
            if(!instance::io::hasCatalogObject(oid))
            {
                LOGD(logname{"rid", supplier._rid, supplier._number} << " new release: " << utils::b2h(oid));
                addIncomplete(oid, Destiny::catalog);
            }
        };

        supplier._api->getReleases().then() += supplier._sol * [this, &supplier](cmt::Future<Set<Oid>> oids)
        {
            if(!oids.resolvedValue())
            {
                LOGD(logname{"rid", supplier._rid, supplier._number} << " getReleases failed: " << exception::toString(oids.detachException()));
                return;
            }

            for(const Oid& oid : oids.value())
            {
                if(!instance::io::hasCatalogObject(oid))
                {
                    LOGD(logname{"rid", supplier._rid, supplier._number} << " new release: " << utils::b2h(oid));
                    addIncomplete(oid, Destiny::catalog);
                }
            }
        };

        fireWorker();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::addIncomplete(const Oid& oid, Destiny destiny)
    {
        auto dIter = _demands.emplace(oid).first;
        if(!(dIter->_destiny & destiny))
        {
            dIter->_destiny |= destiny;
            fireWorker();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::fixComplete(const Oid& oid, Destiny destiny)
    {
        auto iter = _demands.find(oid);
        if(_demands.end() == iter)
            return;

        iter->_destiny &= ~destiny;
        if(!(iter->_destiny))
        {
            iter->_tol.flush();
            fireWorker();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::fireWorker()
    {
        for(auto dIter{_demands.begin()}; dIter!=_demands.end();)
        {
            if(_demandsProcessing.size() >= _maxWorkersCount)
                break;

            const Demand& demand = *dIter;

            if(demand._delayedTransfersRevived.empty())
            {
                // no revived transfers

                auto& suppliersByNumber{_suppliers.get<SupplierByNumber>()};
                auto sIter = suppliersByNumber.lower_bound(demand._supplierBound);
                if(sIter == suppliersByNumber.end())
                {
                    // no more suppliers to probe
                    ++dIter;
                    continue;
                }
            }

            auto node{_demands.extract(dIter++)};
            _demandsProcessing.insert(std::move(node));

            cmt::spawn() += demand._tol * [this, &demand]
            {
                worker(demand);

                if(!demand._destiny)
                    _demandsProcessing.erase(_demandsProcessing.iterator_to(demand));
                else
                    _demands.insert(_demandsProcessing.extract(_demandsProcessing.iterator_to(demand)));

                fireWorker();
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::worker(const Demand& demand)
    {
        consumer::RecvBuffer recvBuffer{_granulaSize};

        while(!!demand._destiny)
        {
            if(!demand._delayedTransfersRevived.empty())
            {
                auto n = demand._delayedTransfersRevived.extract(demand._delayedTransfersRevived.begin());
                Demand::DelayedTransfer& dt{n.value()};
                dt._sol.flush();

                LOGD(logname{"rid", dt._supplierRid, dt._supplierNum, "oid", demand._oid, demand._destiny} << " reuse existing blobTransfer");

                worker(demand, dt._supplierRid, dt._supplierNum, std::move(dt._api), recvBuffer);
                continue;
            }

            auto& suppliersByNumber{_suppliers.get<SupplierByNumber>()};
            auto sIter = suppliersByNumber.lower_bound(demand._supplierBound);
            if(sIter == suppliersByNumber.end())
            {
                // no more suppliers to probe
                break;
            }

            _demandsProcessing.modify(_demandsProcessing.iterator_to(demand), [&](Demand& rw)
            {
                rw._supplierBound = sIter->_number+1;
            });

            LOGD(logname{"rid", sIter->_rid, sIter->_number, "oid", demand._oid, demand._destiny} << " startBlobTransfer");
            api::BlobTransfer<> api{idl::interface::Initializer{}};
            sIter->_api->startBlobTransfer(demand._oid, api.opposite());

            link::Id rid{sIter->_rid};
            worker(demand, rid, sIter->_number, std::move(api), recvBuffer);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::worker(const Demand& demand, const link::Id& rid, std::size_t num, api::BlobTransfer<> api, consumer::RecvBuffer& recvBuffer)
    {
        try
        {
            for(;;)
            {
                LOGD(logname{"rid", rid, num, "oid", demand._oid, demand._destiny} << " getPiece(" << recvBuffer.payloadSize() << ", " << _granulaSize << ")");
                Opt<Bytes> piece = *api->getPiece(recvBuffer.payloadSize(), _granulaSize);

                if(!piece)
                {
                    auto [iter, emplaced] = demand._delayedTransfersEmpty.emplace(api, rid, num);
                    if(emplaced)
                    {
                        LOGD(logname{"rid", rid, num, "oid", demand._oid, demand._destiny} << " getPiece -> none, await availability");

                        const Demand::DelayedTransfer& dd{*iter};

                        api.methods()->available() += dd._sol * [this, &demand, &dd]
                        {
                            LOGD(logname{"rid", dd._supplierRid, dd._supplierNum, "oid", demand._oid, demand._destiny} << " becomes available");
                            //auto tn = demand._delayedTransfersEmpty.extract(dd._api); // c++23
                            auto tn = demand._delayedTransfersEmpty.extract(demand._delayedTransfersEmpty.find(dd._api));
                            demand._delayedTransfersRevived.insert(std::move(tn));
                            fireWorker();
                        };
                        api.involvedChanged() += dd._sol * [&demand, &dd](bool b)
                        {
                            if(!b)
                            {
                                //demand._delayedTransfersEmpty.erase(dd._api); // c++23
                                if(auto iter{demand._delayedTransfersEmpty.find(dd._api)}; iter!=demand._delayedTransfersEmpty.end())
                                    demand._delayedTransfersEmpty.erase(iter);
                                //demand._delayedTransfersRevived.erase(dd._api); // c++23
                                if(auto iter{demand._delayedTransfersRevived.find(dd._api)}; iter!=demand._delayedTransfersRevived.end())
                                    demand._delayedTransfersRevived.erase(iter);
                            }
                        };
                    }
                    else
                        LOGD(logname{"rid", rid, num, "oid", demand._oid, demand._destiny} << " getPiece -> none");

                    return;
                }

                bool granulated{_granulaSize == piece->size()};

                LOGD(logname{"rid", rid, num, "oid", demand._oid, demand._destiny} << " getPiece -> " << piece->size() << " bytes");
                recvBuffer.push(*std::move(piece));

                if(!granulated)
                    break;
            }

            if(!!(Destiny::catalog & demand._destiny))
            {
                instance::io::PutObjectResult res = instance::io::putCatalogObject(demand._oid, recvBuffer.detachBytes());
                switch(res)
                {
                case instance::io::PutObjectResult::ok:
                    LOGD(logname{"oid", demand._oid, demand._destiny} << " putted to catalog");
                    break;
                case instance::io::PutObjectResult::corrupted:
                    LOGD(logname{"oid", demand._oid, demand._destiny} << " corrupted for catalog");
                    recvBuffer.reset();
                    return;
                case instance::io::PutObjectResult::unwanted:
                    LOGD(logname{"oid", demand._oid, demand._destiny} << " unwanted for catalog");
                    break;
                }
            }

            if(!!(Destiny::storage & demand._destiny))
            {
                instance::io::PutObjectResult res;
                if(recvBuffer.hasFile())
                    res = instance::io::putStorageObject(demand._oid, recvBuffer.getFile());
                else
                    res = instance::io::putStorageObject(demand._oid, recvBuffer.detachBytes());

                switch(res)
                {
                case instance::io::PutObjectResult::ok:
                    LOGD(logname{"oid", demand._oid, demand._destiny} << " putted to storage");
                    break;
                case instance::io::PutObjectResult::corrupted:
                    LOGD(logname{"oid", demand._oid, demand._destiny} << " corrupted for storage");
                    recvBuffer.reset();
                    return;
                case instance::io::PutObjectResult::unwanted:
                    LOGD(logname{"oid", demand._oid, demand._destiny} << " unwanted for storage");
                    break;
                }
            }

            if(!!demand._destiny)
            {
                _demandsProcessing.modify(_demandsProcessing.iterator_to(demand), [&](Demand& rw)
                {
                    rw._destiny = {};
                });
            }
            return;
        }
        catch(...)
        {
            LOGD(logname{"rid", rid, num, "oid", demand._oid, demand._destiny} << " exception " << exception::toString(std::current_exception()));
        }
    }
}
