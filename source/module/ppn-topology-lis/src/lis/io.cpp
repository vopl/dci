// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "io.hpp"
#include "../lis.hpp"

namespace dci::module::ppn::topology::lis
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::Io(Lis* lis)
        : _lis{lis}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::~Io()
    {
        _onlines.clear();
        _consumers.clear();
        _suppliers.clear();
        _nextSupplier4GridRequest = _suppliers.end();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::start()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::stop()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 Io::amount4NearsSubscription() const
    {
        return _amount4NearsSubscription;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::state(const space::Id& id, real64 rating, List<transport::Address>&& addresses)
    {
        auto iter = _onlines.find(id);
        if(_onlines.end() == iter)
        {
            return;
        }

        io::Online& online = iter->second;
        online.state(rating, std::move(addresses));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::discovered(const space::Id& id, transport::Address&& address)
    {
        if(_onlines.contains(id))
        {
            //будет проведен через онлайн
            return;
        }

        List<transport::Address> as;
        as.emplace_back(std::move(address));

        std::set<space::Id> processed;
        processed.insert(id);

        space::SmallNum radius = space::dist(id, _lis->localId());
        enumerateNears(_consumers,
                       id,
                       radius,
                       _amount4Discovered,
                       [&](io::Consumer& consumer)
        {
            dbgAssert(radius > space::dist(id, consumer.id()));

            if(!processed.insert(consumer.id()).second)
            {
                return false;
            }

            if(!consumer.started() || (consumer.gridRequested() && !consumer.requestedByGrid(id)))
            {
                return false;
            }

            std::size_t filteredAsAlreadySupplied = 0;
            return consumer.supply(id, as, &filteredAsAlreadySupplied) || filteredAsAlreadySupplied;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::joined(const space::Id& id, const node::link::Remote<>& r)
    {
        Io* io = this;
        io::Online& online = _onlines.emplace(std::piecewise_construct_t{},
                                              std::tie(id),
                                              std::tie(io, id)).first->second;

        bool empty = online.empty();
        online.joined(r);

        if(empty)
        {
            _lis->requestState(id);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::disjoined(const space::Id& id, const node::link::Remote<>& r)
    {
        auto iter = _onlines.find(id);
        if(_onlines.end() == iter)
        {
            return;
        }

        io::Online& online = iter->second;
        online.disjoined(r);

        if(online.empty())
        {
            _onlines.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::Supplier<>::Opposite Io::allocConsumer(const space::Id& id, const node::link::Remote<>& r)
    {
        Io* io = this;
        io::Consumer& consumer = _consumers.emplace(std::piecewise_construct_t{},
                                                    std::tie(id),
                                                    std::tie(io, id)).first->second;
        api::Supplier<>::Opposite s = consumer.alloc(r);

        s.involvedChanged() += consumer.sbsOwner() * [id,s=s.weak(),this](bool v)
        {
            if(!v)
            {
                freeConsumer(id, s);
            }
        };

        return s;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::freeConsumer(const space::Id& id, const api::Supplier<>::Opposite& s)
    {
        auto iter = _consumers.find(id);
        if(_consumers.end() == iter)
        {
            return;
        }

        io::Consumer& consumer = iter->second;
        consumer.free(s);

        if(consumer.empty())
        {
            _consumers.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::allocSupplier(const space::Id& id, const node::link::Remote<>& r, const api::Supplier<>& s)
    {
        io::Supplier& supplier = _suppliers.emplace(std::piecewise_construct_t{},
                                                    std::tie(id),
                                                    std::forward_as_tuple(this)).first->second;
        supplier.alloc(s, r);

        s.involvedChanged() += supplier.sbsOwner() * [id,s=s.weak(),this](bool v)
        {
            if(!v)
            {
                freeSupplier(id, s);
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::freeSupplier(const space::Id& id, const api::Supplier<>& s)
    {
        auto iter = _suppliers.find(id);
        if(_suppliers.end() == iter)
        {
            return;
        }

        io::Supplier& supplier = iter->second;
        supplier.free(s);

        if(supplier.empty())
        {
            if(_nextSupplier4GridRequest == iter)
            {
                _nextSupplier4GridRequest = _suppliers.erase(iter);
            }
            else
            {
                _suppliers.erase(iter);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::supplied(const transport::Address& remoteAddr, const space::Id& remoteId, List<transport::Address>&& as)
    {
        for(transport::Address& a : as)
        {
            if(utils::uri::isCover(remoteAddr.value, a.value))
            {
                _lis->supplied(remoteId, std::move(a));
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::started(io::Consumer* consumer)
    {
        std::set<space::Id> processed;
        processed.insert(consumer->id());

        auto processor = [&](io::Online& online)
        {
            if(!online.charged())
            {
                return false;
            }

            if(!processed.insert(online.id()).second)
            {
                return false;
            }

            if(consumer->gridRequested() && !consumer->requestedByGrid(online.id()))
            {
                return false;
            }

            return consumer->supply(online.id(), online.addresses());
        };

        enumerateNears(
                    _onlines,
                    consumer->id(),
                    (~space::SmallNum{})/2,
                    _amount4NearsSubscription,
                    processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::charged(io::Online* online)
    {
        return changed(online);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::changed(io::Online* online)
    {
        dbgAssert(online->charged());

        std::set<space::Id> processed;
        processed.insert(online->id());

        auto processor = [&](io::Consumer& consumer)
        {
            if(!processed.insert(consumer.id()).second)
            {
                return false;
            }

            if(!consumer.started() || (consumer.gridRequested() && !consumer.requestedByGrid(online->id())))
            {
                return false;
            }

            return consumer.supply(online->id(), online->addresses());
        };

        enumerateNears(
                    _consumers,
                    online->id(),
                    (~space::SmallNum{})/2,
                    _amount4NearsSubscription,
                    processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::remoteGridRequest(io::Consumer* consumer, const io::Consumer::GridRequest& request)
    {
        std::size_t amount = _amount4GridRequest;
        for(auto iter = request.rbegin(); amount && iter!=request.rend(); ++iter)
        {
            std::size_t startIdx = iter->first;
            std::size_t stopIdx = iter->second;

            if(stopIdx > consumer->gridKernel().size()/2)
            {
                amount -= remoteGridRequest(
                              consumer,
                              startIdx,
                              stopIdx,
                              amount);
            }
            else
            {
                for(std::size_t idx{stopIdx-1}; amount && idx>=startIdx && idx<stopIdx; --idx)
                {
                    amount -= remoteGridRequest(
                                  consumer,
                                  idx,
                                  idx+1,
                                  std::min(amount, std::size_t{_amount4GridRequestOneStep}));
                }
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Io::remoteGridRequest(io::Consumer* consumer, std::size_t minGridIdx, std::size_t maxGridIdx, std::size_t amount)
    {
        dbgAssert(amount);
        if(!amount)
        {
            return 0;
        }

        const grid::Kernel& gridKernel = consumer->gridKernel();

        if(minGridIdx >= gridKernel.size() || maxGridIdx > gridKernel.size())
        {
            return 0;
        }

        if(minGridIdx >= maxGridIdx)
        {
            return 0;
        }

        std::set<space::Id> processed;
        processed.insert(consumer->id());

        space::Csid csidMin = gridKernel.csid(maxGridIdx);
        space::Csid csidMax = gridKernel.csid(minGridIdx);
        dbgAssert(csidMax > csidMin);

        std::size_t res {};
        for(const space::Range& range : space::ranges(consumer->id(), csidMin, csidMax))
        {
            dbgAssert(amount > res);
            dbgAssert(range._max > range._min);

            space::SmallNum radius = (range._max - range._min)/2 + 1;
            space::Id center = space::id(static_cast<space::Sid>(range._min + radius));

            res += enumerateNears(_onlines,
                                  center,
                                  radius,
                                  amount - res,
                                  [&](io::Online& online)
            {
                if(!online.charged())
                {
                    return false;
                }

                dbgAssert(consumer->requestedByGrid(online.id()));

                if(!processed.insert(online.id()).second)
                {
                    return false;
                }

                return consumer->supply(online.id(), online.addresses());
            });

            if(res >= amount)
            {
                break;
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::ownGridRequest(const grid::Kernel& gridKernel, const List<api::GridFillingStep>& filling)
    {
        if(_suppliers.empty())
        {
            return;
        }

        for(std::size_t i{0}; i<_suppliers.size(); ++i)
        {
            if(_nextSupplier4GridRequest == _suppliers.end())
            {
                _nextSupplier4GridRequest = _suppliers.begin();
            }

            io::Supplier& supplier = _nextSupplier4GridRequest->second;
            ++_nextSupplier4GridRequest;

            if(!supplier.empty())
            {
                supplier.gridRequest(gridKernel, filling);
                return;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Ring, class F>
    std::size_t Io::enumerateNears(Ring& ring, space::Id center, space::SmallNum radius, std::size_t amount, F&& f)
    {
        if(ring.empty())
        {
            return 0;
        }

        typename Ring::iterator fwdBegin = ring.lower_bound(center);
        if(ring.end() == fwdBegin) fwdBegin = ring.begin();

        typename Ring::reverse_iterator bwdBegin(fwdBegin);
        if(ring.rend() == bwdBegin) bwdBegin = ring.rbegin();

        if(bwdBegin->first == fwdBegin->first)
        {
            ++bwdBegin;
            if(ring.rend() == bwdBegin) bwdBegin = ring.rbegin();
        }

        typename Ring::iterator fwdIter = fwdBegin;
        typename Ring::reverse_iterator bwdIter = bwdBegin;

        space::Sid centerSid = space::sid(center);
        space::SmallNum fwdDist{space::sid(fwdIter->first) - centerSid};
        space::SmallNum bwdDist{centerSid - space::sid(bwdIter->first)};

        std::size_t res {};
        while(res < amount)
        {

            typename Ring::mapped_type* item;

            if(fwdDist <= bwdDist && fwdDist < radius)
            {
                item = &fwdIter->second;

                ++fwdIter;
                if(ring.end() == fwdIter) fwdIter = ring.begin();
                if(fwdIter == fwdBegin)
                {
                    fwdDist = radius;
                }
                else
                {
                    fwdDist = space::sid(fwdIter->first) - centerSid;
                }
            }
            else if(bwdDist < radius)
            {
                item = &bwdIter->second;

                ++bwdIter;
                if(ring.rend() == bwdIter) bwdIter = ring.rbegin();
                if(bwdIter == bwdBegin)
                {
                    bwdDist = radius;
                }
                else
                {
                    bwdDist = centerSid - space::sid(bwdIter->first);
                }
            }
            else
            {
                break;
            }

            if(f(*item))
            {
                ++res;
            }
        }

        return res;
    }
}
