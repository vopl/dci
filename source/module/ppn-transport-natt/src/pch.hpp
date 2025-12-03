// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/config.hpp>
#include <dci/poll/timer.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/s2f.hpp>
#include <dci/utils/endian.hpp>
#include <dci/poll/waitableTimer.hpp>
#include <mutex> //std::lock_guard
#include <regex>
#include <functional>
#include <charconv>
#include <experimental/random>
#include "ppn/transport/natt.hpp"

namespace dci::module::ppn::transport
{
    using namespace dci;

    namespace net   = idl::gen::net;
    namespace ppn   = idl::gen::ppn;
    namespace apit  = idl::gen::ppn::transport;
    namespace api   = idl::gen::ppn::transport::natt;
}
