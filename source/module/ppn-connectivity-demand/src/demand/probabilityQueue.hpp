// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "element.hpp"

namespace dci::module::ppn::connectivity::demand
{
    class ProbabilityQueue
    {
    public:
        ProbabilityQueue();
        ~ProbabilityQueue();

        void insert(const Element* e);
        void update(const std::set<const Element *>& es);
        void delete_(const Element* e);
        void clear();

        const Element* sample();

    private:
        void update();

    private:
        //TODO пока просто заглушка, сюда надо нормальную "очередь с вероятностными приоритетами"
        std::deque<const Element *> _elements;
        real64                      _totalPriority{};
    };
}
