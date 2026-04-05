/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "bucket.hpp"

#include <dci/utils/bits.hpp>

namespace dci::poll::impl::clocking
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bucket::Bucket(std::size_t* globalAmountPtr, std::size_t order, PointRep start)
        : _globalAmountPtr(globalAmountPtr)
        , _order(order)
        , _start(start)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bucket::~Bucket()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Bucket::insert(BucketElement* el)
    {
        dbgAssert(!el->_bucket);

        dbgAssert(el->_point >= this->_start);
        dbgAssert(el->_point < this->_start + (PointRep(1)<<this->_order));

        el->_bucket = this;

        el->_prev = _postLast._prev;
        el->_next = &_postLast;

        _postLast._prev->_next = el;
        _postLast._prev = el;

        (*_globalAmountPtr)++;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Bucket::remove(BucketElement* el)
    {
        dbgAssert(this == el->_bucket);
        el->_bucket = nullptr;

        el->_prev->_next = el->_next;
        el->_next->_prev = el->_prev;

        (*_globalAmountPtr)--;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PointRep Bucket::start() const
    {
        return _start;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Bucket::flushTo(Bucket* to)
    {
        if(_preFirst._next == &_postLast)
        {
            //source us empty, nothing to flush
            return;
        }

        BucketElement* first = _preFirst._next;
        BucketElement* last = _postLast._prev;

        if(to->_preFirst._next == &to->_postLast)
        {
            //destination is empty
            first->_prev = &to->_preFirst;
            to->_preFirst._next = first;

            last->_next = &to->_postLast;
            to->_postLast._prev = last;
        }
        else
        {
            //destination is not empty

            //BucketElement* toFirst = to->_preFirst._next;
            BucketElement* toLast = to->_postLast._prev;

            toLast->_next = first;
            first->_prev = toLast;

            last->_next = &to->_postLast;
            to->_postLast._prev = last;
        }

#ifndef NDEBUG
        to->each([to](BucketElement* be)
        {
            dbgAssert(be->_point >= to->_start);
            dbgAssert(be->_point < to->_start + (PointRep(1)<<to->_order));
        });
#endif

        //clean source
        _preFirst._next = &_postLast;
        _postLast._prev = &_preFirst;

        //adopt amounts and owners
        std::size_t amount = 0;

        BucketElement* iter = first;
        BucketElement* end = last->_next;

        for(; iter!=end; iter=iter->_next)
        {
            dbgAssert(this == iter->_bucket);

            iter->_bucket = to;
            amount++;
        }

        *_globalAmountPtr -= amount;
        *to->_globalAmountPtr += amount;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Bucket::separateChildren(Bucket* readyBucket, BucketPtr children[], PointRep targetTime, PointRep& nearestPoint)
    {
        dbgAssert(this == children[_order].get());

#ifndef NDEBUG
        for(std::size_t i(0); i<_order; ++i)
        {
            dbgAssert(!children[i]);
        }
#endif

        BucketElement* iter = _preFirst._next;
        BucketElement* end = &_postLast;

        while(iter!=end)
        {
            DurationRep offset = iter->_point - _start;
            std::size_t childOrder = utils::bits::bitsof<DurationRep>() - utils::bits::count0Most(offset);

            Bucket* targetBucket;

            if(!childOrder)
            {
                (*_globalAmountPtr)--;
                (*readyBucket->_globalAmountPtr)++;

                targetBucket = readyBucket;
            }
            else
            {
                if(targetTime < iter->_point && iter->_point < nearestPoint)
                {
                    nearestPoint = iter->_point;
                }

                childOrder--;

                dbgAssert(childOrder < _order);

                BucketPtr& child = children[childOrder];

                if(!child)
                {
                    PointRep childStart = _start + (DurationRep(1)<<childOrder);
                    child.reset(new Bucket{_globalAmountPtr, childOrder, childStart});
                }

                targetBucket = child.get();
            }

            BucketElement* el = iter;
            iter=iter->_next;

            dbgAssert(el->_point >= targetBucket->_start);
            dbgAssert(el->_point < targetBucket->_start + (PointRep(1)<<targetBucket->_order));

            dbgAssert(this == el->_bucket);
            el->_bucket = targetBucket;

            el->_prev = targetBucket->_postLast._prev;
            el->_next = &targetBucket->_postLast;

            targetBucket->_postLast._prev->_next = el;
            targetBucket->_postLast._prev = el;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bucket::getNearestPoint(PointRep& nearestPoint)
    {
        BucketElement* iter = _preFirst._next;
        BucketElement* end = &_postLast;

        if(iter == end)
        {
            return false;
        }

        while(iter != end)
        {
            if(nearestPoint > iter->_point)
            {
                nearestPoint = iter->_point;
            }

            iter = iter->_next;
        }

        return true;
    }

}
