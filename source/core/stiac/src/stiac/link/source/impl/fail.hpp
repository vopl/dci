// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <string>

namespace dci::stiac::link::source::impl
{
    class Fail final
    {
    public:
        Fail(const char* cszDetails);
        ~Fail();

        const std::string& details() const;

    private:
        std::string _details;

    };
}
