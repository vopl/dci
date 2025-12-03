// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::discovery
{
    class Peer;
}

namespace dci::module::ppn::discovery::peer
{
    class Io
    {
    public:
        Io(Peer* srv, const node::link::Id& id);
        ~Io();

        const transport::Address& address() const;

        void joined(const node::link::Remote<>& r);
        api::Supplier<> getSupplierOut();
        void declared(const transport::Address& a);

        void regularGet();

    private:
        Peer *                      _srv {};
        node::link::Id              _id {};

        transport::Address          _address;
        api::Supplier<>             _supplierIn;
        api::Supplier<>::Opposite   _supplierOut;

        sbs::Owner _sol;
    };
}
