// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/bytes.hpp>
#include "../smallIntegral.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, const Bytes& v)
    {
        ar << smallIntegral(v.size());

        bytes::Cursor m(v.begin());
        while(m.size())
        {
            ar.write(m.continuousData(), m.continuousDataSize());
            m.advanceChunks(1);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, Bytes&& v)
    {
        ar << smallIntegral(v.size());

        if(v.size())
        {
            ar.write(std::move(v));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void load(auto& ar, Bytes& v)
    {
        uint32 size;
        ar >> smallIntegral(size);
        if(size > ar.size())
        {
            ar.fail("malformed input stream (low data)");
        }

        v.clear();
        if(!size)
        {
            return;
        }

        ar.read(v, size);
    }
}
