// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "config.hpp"

namespace dci::mm::impl::stack
{
    struct Header
    {
        char* _userspaceBegin;
        char* _userspaceMapped;
        char* _userspaceEnd;

#ifdef HAVE_VALGRIND
        unsigned _valgrindId;
#endif

    };
}
