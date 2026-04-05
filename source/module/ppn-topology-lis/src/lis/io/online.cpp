/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "online.hpp"
#include "../io.hpp"

namespace dci::module::ppn::topology::lis::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Online::Online(Io* io, const space::Id& id)
        : _io{io}
        , _id{id}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Online::~Online()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const space::Id& Online::id() const
    {
        return _id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::joined(const node::link::Remote<>& r)
    {
        bool wasCharged = charged();

        _remotes.insert(r);

        if(!wasCharged && charged())
        {
            _io->charged(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::disjoined(const node::link::Remote<>& r)
    {
        _remotes.erase(r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::state(real64 rating, List<transport::Address>&& addresses)
    {
        bool wasCharged = charged();

        _rating = rating;

        List<transport::Address> prevAddresses{std::move(addresses)};
        _addresses.swap(prevAddresses);

        if(charged())
        {
            if(!wasCharged)
            {
                _io->charged(this);
            }
            else if(_addresses != prevAddresses)
            {
                _io->changed(this);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Online::empty() const
    {
        return _remotes.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Online::charged() const
    {
        return _rating > 0 && !_addresses.empty() && !_remotes.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const List<transport::Address>& Online::addresses() const
    {
        return _addresses;
    }
}
