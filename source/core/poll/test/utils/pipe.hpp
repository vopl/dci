// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/poll/descriptor/native.hpp>

namespace utils
{
    int pipe(dci::poll::descriptor::Native::Value sv[2]);
}
