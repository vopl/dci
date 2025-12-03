// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/utils/intrusiveDlist.hpp>
#include <cstdint>

namespace dci::cmt::impl
{
    class Waitable;
}

namespace dci::cmt::impl::details
{
    class Waiter;
}

namespace dci::cmt::details
{

    struct WWLink
        : utils::IntrusiveDlistElement<WWLink>
    {
        impl::details::Waiter * _waiter {};
        impl::Waitable *        _waitable {};
        enum class State
        {
            regular,
            regularConnected,
            repeat,
        }                       _state{State::regular};
    };
}
