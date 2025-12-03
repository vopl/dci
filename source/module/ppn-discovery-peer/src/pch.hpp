// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/logger.hpp>
#include <dci/poll/timer.hpp>
#include <dci/utils/uri.hpp>
#include <dci/config.hpp>
#include "ppn/discovery/peer.hpp"

namespace dci::module::ppn::discovery
{
    using namespace dci;

    namespace transport     = idl::gen::ppn::transport;
    namespace node          = idl::gen::ppn::node;
    namespace api           = idl::gen::ppn::discovery::peer;
}
