// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "iterator.hpp"
#include <stdexcept>
#include <string>

namespace dci::idl::prs
{
    class Error
        : public std::runtime_error
    {
    public:
        Error(const Error&) = default;
        explicit Error(const std::string_view& msg, Iterator pos);
        ~Error() override;

        Error& operator=(const Error&) = default;

        const Iterator& pos() const;

    private:
        Iterator _pos;
    };
}
