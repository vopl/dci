// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/poll/timer.hpp>
#include <dci/logger.hpp>
#include <dci/config.hpp>
#include "ppn/connectivity/demand.hpp"

namespace dci::module::ppn::connectivity
{
    using namespace dci;

    namespace node          = idl::gen::ppn::node;
    namespace api           = idl::gen::ppn::connectivity::demand;
}
