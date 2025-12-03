// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/cmt.hpp>
#include <dci/config.hpp>
#include <dci/poll.hpp>
#include <dci/crypto/rnd.hpp>
#include <dci/crypto/blake2b.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include <dci/utils/atScopeExit.hpp>
#ifdef _WIN32
#   include <dci/utils/win32/error.hpp>
#endif

#include <regex>
#include <functional>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include "ppn/node.hpp"

namespace dci::module::ppn
{
    using namespace dci;

    namespace api       = idl::gen::ppn::node;
    namespace transport = idl::gen::ppn::transport;
    namespace net       = idl::gen::net;
}
