// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "bucketElement.hpp"
#include "config.hpp"

#include <dci/mm/heap/allocable.hpp>

#include <memory>

namespace dci::poll::impl::clocking
{

    struct Bucket;
    using BucketPtr = std::unique_ptr<Bucket>;

    struct Bucket
        : public dci::mm::heap::Allocable<Bucket>
    {
        Bucket(const Bucket&) = delete;
        void operator=(const Bucket&) = delete;

    public:
        Bucket(std::size_t* globalAmountPtr, std::size_t order, PointRep start);
        ~Bucket();

        void insert(BucketElement* el);
        void remove(BucketElement* el);

        PointRep start() const;

        template <class F>
        void each(F&& f);

        template <class F>
        void flush(F&& f);

        void flushTo(Bucket* to);

        void separateChildren(Bucket* readyBucket, BucketPtr children[], PointRep targetTime, PointRep& nearestPoint);

        bool getNearestPoint(PointRep& nearestPoint);

    private:
        std::size_t* _globalAmountPtr;
        std::size_t _order;
        PointRep _start;

        BucketElement _preFirst {&_postLast, nullptr};
        BucketElement _postLast {nullptr, &_preFirst};
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    void Bucket::each(F&& f)
    {
        BucketElement* iter = _preFirst._next;
        BucketElement* end = &_postLast;

        while(iter != end)
        {
            f(iter);
            iter = iter->_next;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    void Bucket::flush(F&& f)
    {
        while(_preFirst._next != &_postLast)
        {
            BucketElement* el = _preFirst._next;

            dbgAssert(this == el->_bucket);
            el->_bucket = nullptr;

            el->_next->_prev = el->_prev;
            el->_prev->_next = el->_next;

            (*_globalAmountPtr)--;

            f(el);
        }

        dbgAssert(!*_globalAmountPtr);

        return;
    }

}
