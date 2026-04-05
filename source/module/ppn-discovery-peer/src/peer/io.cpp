/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "io.hpp"
#include "../peer.hpp"

namespace dci::module::ppn::discovery::peer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::Io(Peer* srv, const node::link::Id& id)
        : _srv{srv}
        , _id{id}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::~Io()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const transport::Address& Io::address() const
    {
        return _address;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::joined(const node::link::Remote<>& r)
    {
        r.involvedChanged() += _sol * [r=r.weak(),this](bool v)
        {
            if(!v)
            {
                _srv->free(r);
            }
        };

        r->remoteAddress().then() += _sol * [r=r.weak(),this](cmt::Future<transport::Address> in)
        {
            if(!in.resolvedValue())
            {
                _srv->free(r);
                return;
            }
            _address = in.detachValue();

            node::link::Remote<>{r}->getInstance(api::Supplier<>::lid()).then() += _sol * [r,this](cmt::Future<idl::Interface> in)
            {
                if(!in.resolvedValue())
                {
                    _srv->free(r);
                    return;
                }
                _supplierIn = in.detachValue();

                _supplierIn->put() += _sol * [this](transport::Address&& a)
                {
                    _srv->input(_address, _id, std::move(a));
                };

                _supplierIn.involvedChanged() += _sol * [r=r.weak(),this](bool v)
                {
                    if(!v)
                    {
                        _supplierIn.reset();
                        _srv->free(r);
                    }
                };

                regularGet();
            };
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::Supplier<> Io::getSupplierOut()
    {
        if(!_supplierOut)
        {
            _supplierOut.init();

            _supplierOut->get() += _sol * [this]
            {
                return cmt::readyFuture(_srv->output(_address));
            };
        }

        return _supplierOut.opposite();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::declared(const transport::Address& a)
    {
        if(_supplierOut)
        {
            _supplierOut->put(a);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::regularGet()
    {
        if(!_supplierIn)
        {
            return;
        }

        _supplierIn->get().then() += _sol * [this](cmt::Future<List<transport::Address>> in)
        {
            if(!in.resolvedValue()) return;
            for(transport::Address& a : in.detachValue())
            {
                _srv->input(_address, _id, std::move(a));
            }
        };
    }
}
