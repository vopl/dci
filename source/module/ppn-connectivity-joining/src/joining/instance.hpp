// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::connectivity
{
    class Joining;
}

namespace dci::module::ppn::connectivity::joining
{
    class Instance
    {
    public:
        Instance(Joining* j, const node::link::Id& id);
        ~Instance();

        const node::link::Id id() const;

        void demand(api::demand::SatisfactionHolder<>&& demand) const;
        void discovered() const;
        void joined(const node::link::Remote<>& r) const;

    private:
        void activate() const;
        void worker() const;

        Joining *                   _j;
        node::link::Id              _id;
        mutable cmt::task::Owner    _taskOwner;

        mutable Map<api::demand::SatisfactionHolder<>, sbs::Owner>  _demands;
        mutable bool                                                _hasAddress{true};
        mutable Map<node::link::Remote<>, sbs::Owner>               _onlines;
    };
}
