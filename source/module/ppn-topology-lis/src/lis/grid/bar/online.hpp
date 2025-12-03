// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../../space.hpp"

namespace dci::module::ppn::topology::lis::grid::bar
{
    class Online
    {
    public:
        Online();
        ~Online();

        bool empty() const;
        std::size_t size() const;

        void joined     (space::Csid csid, const space::Id& id, const node::link::Remote<>& r);
        void disjoined  (space::Csid csid, const space::Id& id, const node::link::Remote<>& r);
        void demand     (space::Csid csid, const space::Id& id, real64 weight, demand::NeedHolder<>&& d);

    private:
        void updateDemand();

    private:
        struct Remote
        {
            space::Id               _id;
            node::link::Remote<>    _api;

            bool operator<(const Remote& other) const
            {
                return _api < other._api;
            }
        };

        struct Record
        {
            std::set<Remote>        _remotes;

            bool                    _demandRequested    {};
            demand::NeedHolder<>    _demand             {};
        };
        using Records = std::map<space::Csid, Record>;
        Records _records;

        static constexpr real64 _demandWeight {1.0};
    };
}
