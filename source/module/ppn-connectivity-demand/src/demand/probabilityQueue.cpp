// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "probabilityQueue.hpp"
#include "element.hpp"
#include <dci/crypto/rnd.hpp>

namespace dci::module::ppn::connectivity::demand
{
    namespace
    {
        struct CmpByTotalPriorityAndPtr
        {
            bool operator()(const Element* a, const Element* b)
            {
                return std::tuple{a->totalPriority(), a} < std::tuple{b->totalPriority(), b};
            }

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            bool operator()(const Element* a, const real64& b) const
            {
                return std::tuple{a->totalPriority(), a} < std::tuple{b, static_cast<const Element *>(nullptr)};
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ProbabilityQueue::ProbabilityQueue()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ProbabilityQueue::~ProbabilityQueue()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProbabilityQueue::insert(const Element* e)
    {
        e->updatePriority();
        e->totalPriority(_totalPriority);

        _elements.push_back(e);
        _totalPriority += e->priority();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProbabilityQueue::update(const std::set<const Element *>& es)
    {
        for(const Element* e : es)
        {
            e->updatePriority();
        }

        update();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProbabilityQueue::delete_(const Element* e)
    {
        auto iter = std::lower_bound(_elements.begin(), _elements.end(), e, CmpByTotalPriorityAndPtr{});

        if(iter != _elements.end())
        {
            dbgAssert(*iter == e);
            iter = _elements.erase(iter);
            update();
        }
        else
        {
            dbgAssert(_elements.empty());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProbabilityQueue::clear()
    {
        _elements.clear();
        _totalPriority = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Element* ProbabilityQueue::sample()
    {
        if(_elements.empty() || _totalPriority <= 0)
        {
            return nullptr;
        }

        real64 pos;
        {
            uint64 rnd;
            crypto::rnd::generate(&rnd, sizeof(rnd));
            pos = static_cast<real64>(rnd) * _totalPriority / 0x1p64;
        }

        auto iter = std::lower_bound(_elements.begin(), _elements.end(), pos, CmpByTotalPriorityAndPtr{});

        if(_elements.end() == iter)
        {
            --iter;
        }

        const Element* res = *iter;
        if(res->totalPriority() <= pos)
        {
            return res;
        }

        dbgAssert(_elements.begin() != iter);

        --iter;
        res = *iter;

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProbabilityQueue::update()
    {
        _totalPriority = 0;

        for(auto iter{_elements.begin()}; iter != _elements.end(); ++iter)
        {
            (*iter)->totalPriority(_totalPriority);
            _totalPriority += (*iter)->priority();
        }
    }
}
