// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/logger.hpp>
#include <dci/poll/timer.hpp>
#include <dci/config.hpp>
#include "ppn/connectivity/joining.hpp"

namespace dci::module::ppn::connectivity
{
    using namespace dci;

    namespace transport = idl::gen::ppn::transport;
    namespace node      = idl::gen::ppn::node;
    namespace rdb       = idl::gen::ppn::node::rdb;
    namespace api       = idl::gen::ppn::connectivity;
}
