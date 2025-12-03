// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>


namespace dci::aup::applier
{
    enum Result : uint64
    {
        rOk                = 0,

        rCorruptedCatalog  = 0x01,
        rAmbiguousCatalog  = 0x02,
        rIncompleteCatalog = 0x04,
        rIncompleteStorage = 0x08,
        rSomeFailed        = 0x0f,

        rExistsMissings    = 0x10,
        rExistsChanges     = 0x20,
        rExistsExtra       = 0x40,
        rSomeWrong         = 0xf0,

        rFixedMissings     = 0x100,
        rFixedChanges      = 0x200,
        rFixedExtra        = 0x400,
    };
}
