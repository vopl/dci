// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "config.hpp"

namespace dci::poll::impl::clocking
{
    struct Bucket;

    struct BucketElement
    {
        BucketElement* _next = nullptr;
        BucketElement* _prev = nullptr;
        Bucket* _bucket = nullptr;

        PointRep _point = PointRep();
    };
}
