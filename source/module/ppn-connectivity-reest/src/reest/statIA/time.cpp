// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "time.hpp"

namespace dci::module::ppn::connectivity::reest::statIA
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TimePoint now()
    {
        return std::chrono::steady_clock::now();
    }
}
