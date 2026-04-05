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
#include "../io.hpp"

namespace dci::module::ppn::topology::lis::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Supplier::Supplier(Io* io)
        : _io{io}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Supplier::~Supplier()
    {
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner& Supplier::sbsOwner()
    {
        return _sbsOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Supplier::alloc(const api::Supplier<>& s, const node::link::Remote<>& r)
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

        _remote = s;
        _gridKernelSent = false;

        _remote->supply() += _sbsOwner * [this](const space::Id& id, List<transport::Address>&& as)
        {
            _io->supplied(_address, id, std::move(as));
        };

        _remote->subscribeNears();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Supplier::free(const api::Supplier<>& s)
    {
        if(_remote == s)
        {
            _sbsOwner.flush();
            _remote.reset();
            _address = {};
            _gridKernelSent = false;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Supplier::empty() const
    {
        return !_remote;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const transport::Address& Supplier::address() const
    {
        return _address;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Supplier::gridRequest(const grid::Kernel& gridKernel, const List<api::GridFillingStep>& filling)
    {
        dbgAssert(_remote);

        if(!_remote)
        {
            return;
        }

        if(!_gridKernelSent || _gridKernel != gridKernel)
        {
            _gridKernel = gridKernel;
            _gridKernelSent = true;
            _remote->gridSetup(_gridKernel.bits(), _gridKernel.size());
        }

        _remote->gridRequest(filling);
    }
}
