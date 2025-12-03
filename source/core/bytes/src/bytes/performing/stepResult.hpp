// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::bytes::performing
{
    template <class T>
    struct StepResult
    {
        StepResult(bool finish, T value);

        const bool _finish;
        const T _value;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    StepResult<T>::StepResult(bool finish, T value)
        : _finish(finish)
        , _value(value)
    {
    }
}
