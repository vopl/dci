// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include <dci/poll/waitableTimer.hpp>
#include "ppn/transport/net.hpp"

namespace dci::module::ppn::transport::net
{
    using namespace dci;

    namespace api = idl::gen::ppn::transport::net;
    namespace apit = idl::gen::ppn::transport;
}
