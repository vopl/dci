// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::bytes::impl
{
    class Alter;
}

namespace dci::bytes::performing
{
    class AlterPromotor
    {
    public:
        AlterPromotor(impl::Alter& alter);
        ~AlterPromotor();

        uint32 possibleContinuousSize();
        void promotePrepare(uint32 size);
        void* continuousData();
        void promoteFix(uint32 size);

    private:
        impl::Alter& _alter;

        void* _writeBuffer = nullptr;
        uint32 _writeBufferSize = 0;
    };
}
