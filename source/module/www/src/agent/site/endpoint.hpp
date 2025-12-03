// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::agent::site
{
    struct Endpoint
    {
        std::string _host;
        uint16      _port;
        bool        _secure{};
        auto operator<=>(const Endpoint&) const = default;
    };
}
