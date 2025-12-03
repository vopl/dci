// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "grid/kernel.hpp"
#include "grid/bar.hpp"
#include "space.hpp"

namespace dci::module::ppn::topology
{
    class Lis;
}

namespace dci::module::ppn::topology::lis
{
    class Grid
    {
    public:
        Grid(Lis* lis);
        ~Grid();

        void start();
        void stop();

    public:
        void setup(uint8 bits, uint16 size);
        const grid::Kernel& kernel() const;
        bool fillRequest(List<api::GridFillingStep>& filling);

    public:
        void joined(const space::Id& id, const node::link::Remote<>& r);
        void disjoined(const space::Id& id, const node::link::Remote<>& r);

        void state(const space::Id& id, real64 rating);

        void demand(const space::Id& id, real64 weight, demand::NeedHolder<>&& d);

    private:
        grid::Bar& bar(space::Csid csid);

    public://for bars
        void requestRatings(space::Csid csidStart, space::Csid csidStop);
        void requestDemand(const space::Id& id, real64 weight);

    private:
        void reindex();

    private:
        Lis *                       _lis;
        grid::Kernel                _kernel;
        std::deque<grid::BarPtr>    _bars;
    };
}
