// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/mm/heap.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::mm::heap
{
    template <class T>
    struct Allocable
    {
        void* operator new(size_t sz)
        {
            dbgAssert(sz == sizeof(T));
            (void)sz;

            return dci::mm::heap::alloc<sizeof(T)>();
        }

        void operator delete(void* ptr)
        {
            dci::mm::heap::free<sizeof(T)>(ptr);
        }
    };
}
