// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
