// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/utils/endian.hpp>

namespace dci::stiac::serialization
{
    template <class T>
    T fixEndian(T v)
    {
        return utils::endian::n2l(v);
    }
}
