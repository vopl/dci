// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
