/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "peer/io.hpp"

namespace dci::module::ppn::discovery
{
    class Peer
        : public idl::gen::ppn::discovery::Peer<>::Opposite
        , public host::module::ServiceBase<Peer>
    {
    public:
        Peer();
        ~Peer();

    private:
        void joined(const node::link::Id& id, const node::link::Remote<>& r);

    private:
        friend class peer::Io;
        void free(const node::link::Remote<>& r);
        void input(const transport::Address& remoteAddr, const node::link::Id& remoteId, transport::Address&& remoteAddr2);
        List<transport::Address> output(const transport::Address& remoteAddr);

    private:
        void regularGet();
        void setIntensity(double v);

    private:
        bool                                _started {};
        Set<transport::Address>             _localAddresses;

    private:
        node::feature::RemoteAddressSpace<> _ras;

        using Ios = Map<node::link::Remote<>, peer::Io>;
        Ios                                 _ios;

        Ios::iterator                       _nextIos4RegularGet;
        poll::Timer                         _regularGetTicker{std::chrono::seconds{1}, true, [this]{regularGet();}};
    };
}
