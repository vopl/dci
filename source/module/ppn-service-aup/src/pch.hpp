// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/aup.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/poll/timer.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/b2h.hpp>
#include <memory>
#include <queue>
#include <deque>
#include <filesystem>
#include <unistd.h>
#include "ppn/service/aup.hpp"

namespace dci::module::ppn::service
{
    using namespace dci;
    using namespace dci::aup;

    namespace link                      = idl::gen::ppn::node::link;
    namespace api_legacy_since_2025_04  = idl::gen::ppn::service::aup_legacy_since_2025_04;
    //namespace api                       = idl::gen::ppn::service::aup;
}
