// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "errorInfo.hpp"

namespace dci::idl::im
{
    ErrorInfo::ErrorInfo(const std::string& message, const PosInSources& pos)
        : _message{message}
        , _pos{pos}
    {
    }

    std::string ErrorInfo::toString() const
    {
        std::string res = _pos.toString();
        if(!res.empty())
        {
            res += ": ";
        }

        res += "error: ";

        if(!_message.empty())
        {
            res += _message;
        }

        return res;
    }
}
