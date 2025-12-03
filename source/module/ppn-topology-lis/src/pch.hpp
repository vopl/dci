// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/logger.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/endian.hpp>
#include <dci/utils/uri.hpp>
#include <dci/poll/timer.hpp>
#include <dci/stiac.hpp>
#include <dci/config.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/poll/timer.hpp>
#include <cmath>
#include "ppn/topology/lis.hpp"

namespace dci::module::ppn::topology
{
    using namespace dci;

    namespace transport     = idl::gen::ppn::transport;
    namespace node          = idl::gen::ppn::node;
    namespace rdb           = idl::gen::ppn::node::rdb;
    namespace pql           = idl::gen::ppn::node::rdb::pql;
    namespace api           = idl::gen::ppn::topology::lis;
    namespace connectivity  = idl::gen::ppn::connectivity;
    namespace demand        = idl::gen::ppn::connectivity::demand;
}
