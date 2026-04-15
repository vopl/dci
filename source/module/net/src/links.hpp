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
#include "link.hpp"

namespace dci::module::net
{
    class Host;

    class Links
        : public sbs::Owner
    {
    public:
        Links(api::Host<>::Opposite* iface);
        ~Links();

    public:
        Link* getLink(uint32 id);
        Link* allocLink(uint32 id);
        void allocatedLinkInitialized(uint32 id, Link* link);

        void delLink(uint32 id);

        void flushChanges(bool complete);

    private:
        using Interfaces        = Map<uint32, api::Link<>>;
        using Implementations   = Map<uint32, std::unique_ptr<Link>>;
        using Ids               = Set<uint32>;

    private:
        api::Host<>::Opposite * _iface = nullptr;

        //work
        Interfaces          _interfaces;
        cmt::Promise<Interfaces>
                            _interfacesInitial;
        Implementations     _implementations;

        //changes
        Implementations     _added;
        Ids                 _deleted;
    };
}
