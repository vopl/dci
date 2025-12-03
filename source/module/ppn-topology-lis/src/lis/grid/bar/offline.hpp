// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../../space.hpp"

namespace dci::module::ppn::topology::lis::grid::bar
{
    class Offline
    {
    public:
        Offline();
        ~Offline();

        void start();
        void stop();

        void rating (space::Csid csid, const space::Id& id, real64 v);
        void demand (space::Csid csid, const space::Id& id, real64 weight, demand::NeedHolder<>&& d);

    private:
        bool _started {false};

    private:
        struct Record
        {
            real64 _rating          {};

            bool                    _demandRequested    {};
            demand::NeedHolder<>    _demand             {};
            real64                  _demandWeight       {};
        };

        std::map<space::Id, Record> _records;
    };
}
