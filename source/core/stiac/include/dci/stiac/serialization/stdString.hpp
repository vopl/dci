// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <string>
#include <dci/primitives.hpp>
#include "../smallIntegral.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, const std::string& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));
        ar.write(v.data(), static_cast<uint32>(v.size()));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void load(auto& ar, std::string& v)
    {
        uint32 size;
        ar >> smallIntegral(size);

        if(size > ar.size())
        {
            ar.fail("malformed input stream (low data)");
        }

        v.resize(size);

        if(v.size())
        {
            ar.read(v.data(), static_cast<uint32>(v.size()));
        }
    }
}
