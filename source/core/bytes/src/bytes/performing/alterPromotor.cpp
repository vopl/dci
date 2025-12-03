// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "alterPromotor.hpp"
#include "../impl/alter.hpp"

namespace dci::bytes::performing
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    AlterPromotor::AlterPromotor(impl::Alter& alter)
        : _alter(alter)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    AlterPromotor::~AlterPromotor()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 AlterPromotor::possibleContinuousSize()
    {
        dbgAssert(!_writeBuffer);
        dbgAssert(!_writeBufferSize);
        _writeBuffer = _alter.prepareWriteBuffer(_writeBufferSize);
        return _writeBufferSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void AlterPromotor::promotePrepare(uint32 size)
    {
        dbgAssert(size <= _writeBufferSize);
        (void)size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void* AlterPromotor::continuousData()
    {
        return _writeBuffer;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void AlterPromotor::promoteFix(uint32 size)
    {
        _writeBuffer = nullptr;
        _writeBufferSize = 0;
        _alter.commitWriteBuffer(size);
    }

}
