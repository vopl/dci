// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "bar/offline.hpp"
#include "bar/online.hpp"
#include "../space.hpp"

namespace dci::module::ppn::topology::lis
{
    class Grid;
}

namespace dci::module::ppn::topology::lis::grid
{
    class Bar
        : public mm::heap::Allocable<Bar>
        , private bar::Offline
        , private bar::Online
    {
    public:
        Bar(Grid* grid, space::Csid csidBegin, space::Csid csidEnd);
        ~Bar();

        void start();
        void stop();

        void joined     (space::Csid csid, const space::Id& id, const node::link::Remote<>& r);
        void disjoined  (space::Csid csid, const space::Id& id, const node::link::Remote<>& r);
        void rating     (space::Csid csid, const space::Id& id, real64 v);
        void demand     (space::Csid csid, const space::Id& id, real64 weight, demand::NeedHolder<>&& d);

        std::size_t onlineSize() const;

    private:
        void requestRatings();
        void requestDemand(const space::Id& id, real64 weight);

    private:
        friend class bar::Offline;
        friend class bar::Online;

    private:
        Grid *      _grid;
        space::Csid _csidBegin{};
        space::Csid _csidEnd{};
    };
    using BarPtr = std::unique_ptr<Bar>;
}
