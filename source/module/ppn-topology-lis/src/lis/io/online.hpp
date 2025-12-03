// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../space.hpp"

namespace dci::module::ppn::topology::lis
{
    class Io;
}

namespace dci::module::ppn::topology::lis::io
{
    class Online
    {
    public:
        Online(Io* io, const space::Id& id);
        ~Online();

        const space::Id& id() const;

        void joined(const node::link::Remote<>& r);
        void disjoined(const node::link::Remote<>& r);

        void state(real64 rating, List<transport::Address>&& addresses);

        bool empty() const;

    public:
        bool charged() const;
        const List<transport::Address>& addresses() const;

    private:
        Io *                        _io;
        space::Id                   _id;
        Set<node::link::Remote<>>   _remotes;
        real64                      _rating{};
        List<transport::Address>    _addresses;
    };
}
