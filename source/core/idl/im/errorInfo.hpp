// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "posInSources.hpp"

namespace dci::idl::im
{
    class ErrorInfo
    {
    public:
        ErrorInfo(const std::string& message, const PosInSources& pos);
        std::string toString() const;

    private:
        std::string     _message;
        PosInSources    _pos;
    };
}
