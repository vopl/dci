// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/idl/interface/id.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::idl::interface
{
    bool Id::fromText(const String& text)
    {
        if(text.size() < _cid._size*2+1)
        {
            return false;
        }

        if(!_cid.fromHex(text))
        {
            return false;
        }

        switch(text.back())
        {
        case 'p':
            _side = Side::primary;
            break;
        case 'o':
            _side = Side::opposite;
            break;
        default:
            return false;
        }

        return true;
    }

    String Id::toText() const
    {
        String text = _cid.toHex();

        switch(_side)
        {
        case Side::primary:
            text += 'p';
            break;
        case Side::opposite:
            text += 'o';
            break;
        default:
            dbgWarn("bad side value in interface::Id");
            text += '?';
            break;
        }

        return text;
    }
}
