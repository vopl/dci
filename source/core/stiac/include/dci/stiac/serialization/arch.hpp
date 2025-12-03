// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/bytes.hpp>

namespace dci::stiac::serialization
{
    struct Arch
        : bytes::Alter
    {
        using bytes::Alter::Alter;
        Arch() = delete;
        Arch(const bytes::Alter& from);
        Arch(bytes::Alter&& from);

        void read(void* data, uint32 size);
        void read(Bytes& data, uint32 size);

        [[noreturn]] void fail(const char* cszDetails);

        Arch& operator<<(auto&& v);
        Arch& operator>>(auto&& v);
    };

}
