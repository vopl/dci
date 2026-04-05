/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
