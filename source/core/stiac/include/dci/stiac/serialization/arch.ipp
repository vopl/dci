// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "arch.hpp"
#include "../serialization.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Arch::Arch(const bytes::Alter& from)
        : bytes::Alter(from)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Arch::Arch(bytes::Alter&& from)
        : bytes::Alter(std::move(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Arch::read(void* data, uint32 size)
    {
        if(size != bytes::Alter::read(data, size))
        {
            fail("low data");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Arch::read(Bytes& data, uint32 size)
    {
        if(size != bytes::Alter::read(data, size))
        {
            fail("low data");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    [[noreturn]] inline void Arch::fail(const char* cszDetails)
    {
        throw std::runtime_error(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Arch& Arch::operator<<(auto&& v)
    {
        using stiac::serialization::save;

        save(*this, std::forward<decltype(v)>(v));
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Arch& Arch::operator>>(auto&& v)
    {
        using stiac::serialization::load;

        load(*this, std::forward<decltype(v)>(v));
        return *this;
    }

}
