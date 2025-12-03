// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "sink.hpp"
#include "../serialization.hpp"
#include "serialization.hpp"

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Sink& Sink::operator<<(auto&& v)
    {
        using stiac::serialization::save;
        using stiac::link::serialization::save;

        save(*this, std::forward<decltype(v)>(v));
        return *this;
    }

}
