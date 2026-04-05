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
#include "../grid/kernel.hpp"
#include "../space.hpp"

namespace dci::module::ppn::topology::lis
{
    class Io;
}

namespace dci::module::ppn::topology::lis::io
{
    class Consumer
    {
    public:
        using IdxRange = std::pair<std::size_t, std::size_t>;
        using GridRequest = std::vector<IdxRange>;//ordered start-stop indices

    public:
        Consumer(Io* io, const space::Id& id);
        ~Consumer();

        const space::Id& id() const;
        sbs::Owner& sbsOwner();

        const grid::Kernel& gridKernel() const;
        bool gridRequested() const;
        bool requestedByGrid(const space::Id& id) const;

        bool started() const;

        api::Supplier<>::Opposite alloc(const node::link::Remote<>& r);
        void free(const api::Supplier<>::Opposite& s);
        bool empty() const;

        const transport::Address& address() const;

        bool supply(const space::Id& id, const List<transport::Address>& as, std::size_t* filteredAsAlreadySupplied = nullptr);

    private:
        Io *                        _io;
        space::Id                   _id;
        sbs::Owner                  _sbsOwner;
        api::Supplier<>::Opposite   _remote;
        transport::Address          _address;

        grid::Kernel                _gridKernel;
        bool                        _gridKernelReceived{false};
        GridRequest                 _lastGridRequested;

        bool                        _started {};

        using ASet = std::set<transport::Address>;
        using ASetIterList = std::deque<ASet::iterator>;

        ASet            _suppliedAddressesSet;
        ASetIterList    _suppliedAddressesList;
    };
}
