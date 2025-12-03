// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::http::client::cookies::path
{
    void directoryFromResource(std::string& path);
    void canonicalize(std::string& path, std::string_view default_);
    bool matched(std::string_view target, std::string_view pattern);
}
