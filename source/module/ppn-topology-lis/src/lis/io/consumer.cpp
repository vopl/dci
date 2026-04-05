/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "supplier.hpp"
#include "../io.hpp"

namespace dci::module::ppn::topology::lis::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Consumer::Consumer(Io* io, const space::Id& id)
        : _io{io}
        , _id{id}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Consumer::~Consumer()
    {
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const space::Id& Consumer::id() const
    {
        return _id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner& Consumer::sbsOwner()
    {
        return _sbsOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const grid::Kernel& Consumer::gridKernel() const
    {
        return _gridKernel;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Consumer::gridRequested() const
    {
        return !_lastGridRequested.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Consumer::requestedByGrid(const space::Id& id) const
    {
        if(!_gridKernelReceived || _lastGridRequested.empty())
        {
            return false;
        }

        std::size_t idx = _gridKernel.idx(space::csid(_id, id));

        auto iter = std::lower_bound(_lastGridRequested.begin(), _lastGridRequested.end(), IdxRange{idx, 0}, [](const IdxRange& a, const IdxRange& b)
        {
            return a.first < b.first;
        });

        if(_lastGridRequested.end() == iter)
        {
            --iter;
        }

        if(_lastGridRequested.begin() != iter && iter->first > idx)
        {
            --iter;
        }

        bool res = iter->first <= idx && idx <= iter->second;
        if(!res)
        {
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Consumer::started() const
    {
        return _started;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::Supplier<>::Opposite Consumer::alloc(const node::link::Remote<>& r)
    {
        _sbsOwner.flush();

        r->remoteAddress().then() += _sbsOwner * [this](cmt::Future<transport::Address> in)
        {
            if(in.resolvedValue())
            {
                _address = in.detachValue();
            }
            else
            {
                _address = {};
            }
        };

        _remote.reset();
        _remote.init();

        _started = false;

        _remote->subscribeNears() += _sbsOwner * [this]
        {
            _started = true;
            _io->started(this);
        };

        _remote->usubscribeNears() += _sbsOwner * [this]
        {
            _started = false;
        };

        _remote->gridSetup() += _sbsOwner * [this](uint8 bits, uint16 size)
        {
            _gridKernelReceived = true;
            _gridKernel.change(bits, size);
            _lastGridRequested.clear();
        };

        _remote->gridRequest() += _sbsOwner * [this](const List<api::GridFillingStep>& filling)
        {
            if(_gridKernelReceived)
            {
                _lastGridRequested.clear();

                std::size_t idx{0};
                for(const api::GridFillingStep& step : filling)
                {
                    idx += step.full;

                    if(step.empty)
                    {
                        _lastGridRequested.emplace_back(idx, idx + step.empty);
                        idx += step.empty;
                    }
                }

                _io->remoteGridRequest(this, _lastGridRequested);
            }
        };

        return _remote;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Consumer::free(const api::Supplier<>::Opposite& s)
    {
        if(_remote == s)
        {
            _sbsOwner.flush();
            _remote.reset();
            _address = {};
            _started = false;
            _suppliedAddressesSet.clear();
            _suppliedAddressesList.clear();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Consumer::empty() const
    {
        return !_remote;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const transport::Address& Consumer::address() const
    {
        return _address;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Consumer::supply(const space::Id& id,  const List<transport::Address>& as, std::size_t* filteredAsAlreadySupplied)
    {
        if(!_started || !_remote)
        {
            return false;
        }

        List<transport::Address> filtered;
        for(const transport::Address& a : as)
        {
            if(!utils::uri::isCover(_address.value, a.value))
            {
                continue;
            }

            auto insertRes = _suppliedAddressesSet.insert(a);
            if(!insertRes.second)
            {
                if(filteredAsAlreadySupplied)
                {
                    ++ *filteredAsAlreadySupplied;
                }
                continue;
            }

            filtered.push_back(std::move(a));

            _suppliedAddressesList.push_back(insertRes.first);
            while(_suppliedAddressesList.size() > 100)
            {
                _suppliedAddressesSet.erase(*_suppliedAddressesList.begin());
                _suppliedAddressesList.erase(_suppliedAddressesList.begin());
            }
        }

        if(filtered.empty())
        {
            return false;
        }

        _remote->supply(id, std::move(filtered));
        return true;
    }
}
