// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node::utils
{
    std::string mkRandomName(size_t chars);

    api::link::Key parseKey(const config::ptree& config);
    uint16 parseUint16(const String& param);
}
