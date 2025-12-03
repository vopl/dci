// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::bytes::impl
{
    class Cursor;
}

namespace dci::bytes::performing
{
    class CursorPromotor
    {
    public:
        CursorPromotor(impl::Cursor& cursor);
        ~CursorPromotor();

        uint32 possibleContinuousSize();
        void promotePrepare(uint32 size);
        const void* continuousData();
        void promoteFix(uint32 size);

    private:
        impl::Cursor& _cursor;
    };
}
