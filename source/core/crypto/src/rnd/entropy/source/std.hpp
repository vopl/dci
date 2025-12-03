// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../source.hpp"
#include <string_view>
#include <random>

namespace dci::crypto::rnd::entropy::source
{
    class Std
        : public Source
    {
    public:
        static bool available();
        static const std::string_view name();

    public:
        Std(Instance* instance);
        ~Std() override;

        void flush() override;

    private:
        std::random_device _rd;
    };
}
