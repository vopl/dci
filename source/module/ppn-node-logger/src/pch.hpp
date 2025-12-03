// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/config.hpp>
#include <dci/logger.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/fnv1a.hpp>
#include <regex>
#include "ppn/node/logger.hpp"

namespace dci::module::ppn::node
{
    using namespace dci;

    namespace api       = idl::gen::ppn::node;
    namespace transport = idl::gen::ppn::transport;
}
