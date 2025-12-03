// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/logger.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/poll/waitableTimer.hpp>
#include <dci/stiac.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include "ppn/discovery/local.hpp"

namespace dci::module::ppn::discovery
{
    using namespace dci;

    namespace transport = idl::gen::ppn::transport;
    namespace link      = idl::gen::ppn::node::link;
    namespace node      = idl::gen::ppn::node;
    namespace api       = idl::gen::ppn::discovery::local;
    namespace net       = idl::gen::net;
}
