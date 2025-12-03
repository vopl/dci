// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "mac.hpp"
#include <utility>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(std::size_t digestSize)
        : Hash{digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(const Mac& from)
        : Hash{from}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(Mac&& from)
        : Hash{std::move(from)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::~Mac()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac& Mac::operator=(const Mac& from)
    {
        Hash::operator=(from);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac& Mac::operator=(Mac&& from)
    {
        Hash::operator=(std::move(from));
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mac::setKey(const void* key, std::size_t len)
    {
        (void)key;
        (void)len;
        //ok
    }
}
