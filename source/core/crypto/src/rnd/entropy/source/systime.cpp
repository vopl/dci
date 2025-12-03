// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "systime.hpp"
#include "../../instance.hpp"
#include <chrono>
#include <iostream>

namespace dci::crypto::rnd::entropy::source
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool SysTime::available()
    {
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string_view SysTime::name()
    {
        return std::string_view("systime");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SysTime::flush()
    {
        auto now = std::chrono::high_resolution_clock::now();
        _instance->addEntropy(&now, sizeof(now), 0);
    }
}
