// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::connectivity::demand
{
    class Registry;

    class Element
        : public dci::mm::heap::Allocable<Element>
    {
    public:
        using Clock = std::chrono::steady_clock;
        using Moment = Clock::time_point;

    public:
        Element(const node::link::Id& id, Registry* r);
        ~Element();

        const node::link::Id& id() const;

        void updatePriority() const;
        real64 priority() const;
        real64 totalPriority() const;
        void totalPriority(real64 v) const;

        bool tryUnban(Moment now) const;

    public:
        api::NeedHolder<> need(real64 weight) const;
        api::SatisfactionHolder<> satisfy() const;

    private:
        node::link::Id                                                              _id;
        mutable Registry *                                                          _r {};

        struct NeedState
        {
            sbs::Owner  _sol;
            real64      _weight{};
        };
        struct SatisfactionState
        {
            sbs::Owner  _sol;
        };

        mutable std::map<api::NeedHolder<>::Opposite, NeedState>                    _needs;
        mutable std::map<api::SatisfactionHolder<>::Opposite, SatisfactionState>    _satisfies;

        mutable std::multiset<Moment>                                               _bans;

    private:
        mutable real64 _priority {};
        mutable real64 _totalPriority {};
    };
}
