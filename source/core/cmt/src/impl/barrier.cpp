// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "barrier.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Barrier::Barrier(std::size_t depth)
        : Waitable(
            [](const Waitable* w){ return static_cast<const Barrier*>(w)->canStride();},
            [](Waitable* w){ return static_cast<Barrier*>(w)->tryStride();})
        , _depth(std::max(depth, std::size_t(1)))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Barrier::~Barrier()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Barrier::tryDestruction(Barrier* b)
    {
        b->~Barrier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Barrier::canStride() const
    {
        return _linksAmount + 1 >= _depth;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Barrier::tryStride()
    {
        throwTaskStopIfNeed();

        if(canStride())
        {
            _links.each([](WWLink* link)
            {
                link->_waiter->readyOffer(link);
            });

            return true;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Barrier::stride()
    {
        if(tryStride())
        {
            return;
        }

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Barrier::wait()
    {
        stride();
    }
}
