/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once
#include <dci/mm/heap/allocable.hpp>
#include <dci/sbs.hpp>
#include <dci/poll.hpp>
#include <dci/logger.hpp>
#include <dci/host/module/entry.hpp>
#include <dci/host/exception.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/win32/error.hpp>
#include <dci/utils/ip.hpp>
#include <dci/utils/overloaded.hpp>
#include <dci/utils/compiler.hpp>
#include "net.hpp"

#include <memory>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <codecvt>

#include <unistd.h>

#ifdef _WIN32
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   define NOCOMM
#   include <ws2tcpip.h>
#   include <afunix.h>
#   include <iphlpapi.h>
#   undef interface

    struct Buf : WSABUF
    {
        static constexpr std::uint32_t _maxBufs = 1024;

        using Len = u_long;
        using Data = char*;

        Len& len() {return WSABUF::len;}
        const Len& len() const {return WSABUF::len;}

        Data& data() {return WSABUF::buf;}
        const Data& data() const {return WSABUF::buf;}
    };
#else
#   include <linux/rtnetlink.h>
#   include <netinet/in.h>
#   include <sys/un.h>
#   include <net/if.h>

#   include <sys/types.h>
#   include <sys/socket.h>
#   include <sys/uio.h>
#   include <netdb.h>
#   include <netinet/tcp.h>

#   include <sys/eventfd.h>

struct Buf : iovec
{
    static constexpr std::uint32_t _maxBufs = UIO_MAXIOV;

    using Len = size_t;
    using Data = void*;

    Len& len() {return iovec::iov_len;}
    const Len& len() const {return iovec::iov_len;}

    Data& data() {return iovec::iov_base;}
    const Data& data() const {return iovec::iov_base;}
};

#endif

namespace dci::module::net
{
    namespace api = dci::idl::gen::net;
}

