// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
