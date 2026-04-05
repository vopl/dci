/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"

namespace dci::bytes::performing
{
    template <class T>
    class ArrayPromotor
    {
        using Element = std::conditional_t<
            std::is_const_v<T>,
            const byte,
            byte
        >;

    public:
        ArrayPromotor(T* data, uint32 size);

        uint32 possibleContinuousSize();
        void promotePrepare(uint32 size);
        Element* continuousData();
        void promoteFix(uint32 size);

    private:
        Element*    _data;
        uint32      _size;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    ArrayPromotor<T>::ArrayPromotor(T* data, uint32 size)
        : _data(static_cast<Element*>(data))
        , _size(size)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    uint32 ArrayPromotor<T>::possibleContinuousSize()
    {
        return _size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void ArrayPromotor<T>::promotePrepare(uint32 /*size*/)
    {
        //empty
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    typename ArrayPromotor<T>::Element* ArrayPromotor<T>::continuousData()
    {
        return _data;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void ArrayPromotor<T>::promoteFix(uint32 size)
    {
        dbgAssert(size <= _size);

        _size -= size;
        if(!_size)
        {
            _data = nullptr;
        }
        else
        {
            _data += size;
        }
    }

}
