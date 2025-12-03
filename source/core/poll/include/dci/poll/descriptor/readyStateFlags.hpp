// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

namespace dci::poll::descriptor
{
    enum ReadyStateFlags : unsigned
    {
        rsf_null    = 0x000,

        rsf_read    = 0x001,
        rsf_pri     = 0x002,
        rsf_write   = 0x004,

        rsf_error   = 0x010,

        rsf_eof     = 0x100,
        rsf_close   = 0x200,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags operator~(ReadyStateFlags a)
    {
        return static_cast<ReadyStateFlags>(~static_cast<unsigned>(a));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags operator&(ReadyStateFlags a, ReadyStateFlags b)
    {
        return static_cast<ReadyStateFlags>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags operator|(ReadyStateFlags a, ReadyStateFlags b)
    {
        return static_cast<ReadyStateFlags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags& operator&=(ReadyStateFlags& a, ReadyStateFlags b)
    {
        a = a & b;
        return a;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags& operator|=(ReadyStateFlags& a, ReadyStateFlags b)
    {
        a = a | b;
        return a;
    }
}
