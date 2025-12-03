// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "mm/heap.hpp"
#include "mm/heap/allocable.hpp"

#include "mm/stack.hpp"

namespace dci::mm
{
    void API_DCI_MM setupPanicHandler(void(*)(int));
}
