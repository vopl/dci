// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

//временно, пока в clangd не подвезли std::expected
#if __cplusplus > 202002L && (!defined(__cpp_concepts) || __cpp_concepts < 202002L)
#   define __cpp_concepts 202002L
#endif

#include <dci/host/module/entry.hpp>
#include <dci/host/manager.hpp>
#include <dci/cmt.hpp>
#include <dci/exception.hpp>
#include <dci/poll/timeout.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/overloaded.hpp>
#include <dci/utils/compiler.hpp>
#include <dci/utils/dns.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include <dci/utils/dbg.hpp>
#include <dci/stiac.hpp>
#include <dci/crypto/blake2b.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <zlib.h>

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef _WIN32
#   ifdef DELETE
#       undef DELETE
#   endif
#   ifdef interface
#       undef interface
#   endif
#endif

#include <bit>
#include <deque>
#include <string_view>
#include <expected>
#include "www.hpp"

namespace dci::module::www
{
    namespace api = dci::idl::gen::www;
    namespace net = idl::gen::net;
}

