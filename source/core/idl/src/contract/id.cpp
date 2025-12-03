// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/idl/contract/id.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/b2h.hpp>

namespace dci::idl::contract
{
    bool Id::fromHex(const String& hex)
    {
        if(hex.size() < _size*2)
        {
            return false;
        }

        return dci::utils::h2b(hex.data(), _size*2, data());
    }

    String Id::toHex(uint32 chars) const
    {
        if(chars > _size*2)
        {
            chars = _size*2;
        }

        String hex;
        hex.resize(chars);

        dci::utils::b2h(data(), _size, hex.data());

        return hex;
    }
}
