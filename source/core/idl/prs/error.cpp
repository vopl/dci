// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "error.hpp"

namespace dci::idl::prs
{
    Error::Error(const std::string_view& msg, Iterator pos)
        : std::runtime_error{std::string{msg}}
        , _pos{pos}
    {
    }

    Error::~Error()
    {
    }

    const Iterator& Error::pos() const
    {
        return _pos;
    }
}
