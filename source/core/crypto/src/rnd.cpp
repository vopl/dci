// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/crypto/rnd.hpp>
#include "rnd/instance.hpp"

namespace dci::crypto::rnd
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool generate(void* buf, std::size_t len)
    {
        static Instance instance;
        return instance.generate(buf, len);
    }
}
