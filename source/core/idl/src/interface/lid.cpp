// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/idl/interface/lid.hpp>
#include <dci/idl/interface/id.hpp>
#include <dci/idl/contract/lidRegistry.hpp>

namespace dci::idl::interface
{
    bool Lid::fromIidText(const String& text)
    {
        Id id;
        if(!id.fromText(text))
        {
            return false;
        }

        _clid = contract::lidRegistry.get(id._cid);
        _side = id._side;

        return true;
    }

    String Lid::toIidText() const
    {
        Id id {contract::lidRegistry.get(_clid), _side};

        return id.toText();
    }
}
