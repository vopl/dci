// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "runtimeExecutors.hpp"

namespace dci::himpl::check
{
    template <class TFace>
    class RuntimeInstance
    {
    public:
        static volatile const int _initiator;
    };

    template <class TFace>
    volatile const int RuntimeInstance<TFace>::_initiator = runtimeCheckFace<TFace>();

}
