// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/poll/timer.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include <dci/poll/waitableTimer.hpp>
#include "ppn/transport.hpp"

namespace dci::module::ppn::transport
{
    using namespace dci;

    namespace api = dci::idl::gen::ppn::transport;
}
