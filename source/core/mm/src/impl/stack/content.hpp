// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "layout.hpp"
#include "config.hpp"

namespace dci::mm::impl::stack
{
    class Content
        : public Layout<Config::_stackGrowsDown, Config::_stackHasGuard>
    {
        using Base = Layout<true, true>;

    public:
        Content();
        ~Content();

    public:
        const Header& header();
    };
}
