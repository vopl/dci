/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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

#include <flat_set>

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

