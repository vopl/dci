// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "keyI.hpp"
#include "statIA.hpp"

namespace dci::module::ppn::connectivity
{
    class Reest;
}

namespace dci::module::ppn::connectivity::reest
{
    class StatI
    {
    public:
        StatI(Reest* srv);
        ~StatI();

        void statChanged(StatIA* stat);
        void statDead(StatIA* stat);
        void onceFlush();

        const std::vector<StatIA*>& top() const;
        bool dead() const;

    private:
        Reest* _srv;
        std::set<StatIA*>       _stats;

        std::vector<StatIA*>    _top;
        bool                    _topChanged{};
        real64                  _rating0{};
    };
}
