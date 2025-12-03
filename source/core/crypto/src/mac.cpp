// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/crypto/mac.hpp>
#include "impl/mac.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(himpl::FakeConstructionArg fc)
        : himpl::FaceLayout<Mac, impl::Mac, Hash>(fc)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::~Mac()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mac::setKey(const void* key, std::size_t len)
    {
        return impl().setKey(key, len);
    }
}
