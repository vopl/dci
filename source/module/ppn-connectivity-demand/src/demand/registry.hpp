/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "element.hpp"
#include "probabilityQueue.hpp"

namespace dci::module::ppn::connectivity
{
    class Demand;
}

namespace dci::module::ppn::connectivity::demand
{
    class Registry
        : public api::Registry<>::Opposite
    {
    public:
        Registry(Demand* d);
        ~Registry();

        void start();
        void stop();

        void setIntensity(double v);

    public:
        const Element& element(const node::link::Id& id);

    private:
        friend class Element;
        void construction(const Element* e);
        void destruction(const Element* e);
        void changed(const Element* e);
        void ban(const Element* e);

    private:
        void flushChanges();
        void makeSatisfies();

    private:
        Demand* _d;
        sbs::Owner _sol;

    private:
        std::set<const Element *>   _changed;
        std::set<const Element *>   _ban;
        poll::Timer                 _flushChangesTicker{std::chrono::milliseconds{0}, false, [this]{flushChanges();}};
        poll::Timer                 _makeSatisfiesTicker{std::chrono::milliseconds{100}, true, [this]{makeSatisfies();}};

    private:
        struct CmpById
        {
            using is_transparent = void;
            bool operator()(const Element& a, const Element& b) const;
            bool operator()(const Element& a, const node::link::Id& b) const;
        };

        using ById = std::set<Element, CmpById>;
        ById                        _byId;
        demand::ProbabilityQueue    _byProbability;
    };
}
