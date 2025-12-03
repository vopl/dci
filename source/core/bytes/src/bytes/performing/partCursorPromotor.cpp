// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "partCursorPromotor.hpp"
#include "../impl/cursor.hpp"

namespace dci::bytes::performing
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PartCursorPromotor::PartCursorPromotor(impl::Cursor& cursor, uint32 maxSize)
        : _cursor(cursor)
        , _maxSize(maxSize)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PartCursorPromotor::~PartCursorPromotor()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 PartCursorPromotor::possibleContinuousSize()
    {
        return std::min(_maxSize, _cursor.continuousDataSize());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void PartCursorPromotor::promotePrepare(uint32 size)
    {
        (void)size;
        dbgAssert(!size == !possibleContinuousSize());
        dbgAssert(size <= possibleContinuousSize());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const void* PartCursorPromotor::continuousData()
    {
        dbgAssert(!_cursor.continuousDataSize() == !_cursor.continuousData());
        return _cursor.continuousData();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void PartCursorPromotor::promoteFix(uint32 size)
    {
        int32 isize = static_cast<int32>(size);
        int32 res = _cursor.advance(isize);
        (void)res;
        dbgAssert(res == isize);

        dbgAssert(_maxSize >= size);
        _maxSize -= size;
    }

}
