// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../source.hpp"
#include <string_view>

namespace dci::crypto::rnd::entropy::source
{
    class RDRAND
        : public Source
    {
    public:
        using Source::Source;
        static bool available();
        static const std::string_view name();
        void flush() override;
    };
}
