// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../grid/kernel.hpp"

namespace dci::module::ppn::topology::lis
{
    class Io;
}

namespace dci::module::ppn::topology::lis::io
{
    class Supplier
    {
    public:
        Supplier(Io* io);
        ~Supplier();

        sbs::Owner& sbsOwner();

        void alloc(const api::Supplier<>& s, const node::link::Remote<>& r);
        void free(const api::Supplier<>& s);
        bool empty() const;

        const transport::Address& address() const;

        void gridRequest(const grid::Kernel& gridKernel, const List<api::GridFillingStep>& filling);

    private:
        Io *                _io;
        sbs::Owner          _sbsOwner;
        api::Supplier<>     _remote;
        transport::Address  _address;
        grid::Kernel        _gridKernel;
        bool                _gridKernelSent{false};
    };
}
