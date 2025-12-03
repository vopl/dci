// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/mm/heap/allocable.hpp>
#include <dci/sbs.hpp>
#include <dci/cmt.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/logger.hpp>
#include <dci/stiac.hpp>
#include <dci/crypto.hpp>
#include <dci/host/module/entry.hpp>
#include <dci/host/exception.hpp>
#include <dci/logger.hpp>
#include <dci/poll/timer.hpp>

#include "stiac.hpp"

#include <queue>
#include <cstring>

#include <zstd.h>

#include <boost/crc.hpp>

namespace dci::module::stiac
{
    using namespace dci;
    using namespace dci::cmt;
    using namespace dci::stiac;
    using namespace dci::idl::gen::stiac;
    using namespace dci::idl;

    namespace api = dci::idl::gen::stiac;
    namespace apip = api::protocol;
    namespace apil = api::localEdge;
}
