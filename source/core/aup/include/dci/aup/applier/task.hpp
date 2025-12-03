// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>


namespace dci::aup::applier
{
    enum Task : uint64
    {
        tNull              = 0x0,
        tVerboseMajor      = 0x1,
        tVerboseMinor      = 0x2,
        tVerbose           = 0x3,
        tCheckStorage      = 0x4,

        tRemoveWrongs      = 0x10,
        tEmplaceMissings   = 0x20,
        tEmplaceChanges    = 0x40,
        tRemoveExtra       = 0x80,

        tAll               = ~uint64{}
    };
}
