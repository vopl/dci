// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "hash.hpp"
#include <dci/utils/dbg.hpp>
#include <cstdlib>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hash::Hash(std::size_t digestSize)
        : _digestSize(digestSize)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hash::Hash(const Hash& from)
        : _digestSize{from._digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hash::Hash(Hash&& from)
        : _digestSize{from._digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hash::~Hash()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hash& Hash::operator=(const Hash& from)
    {
        _digestSize = from._digestSize;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hash& Hash::operator=(Hash&& from)
    {
        _digestSize = from._digestSize;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Hash::digestSize()
    {
        return _digestSize;
    }
}
