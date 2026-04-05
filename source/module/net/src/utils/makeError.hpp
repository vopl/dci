/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once
#include "pch.hpp"

namespace dci::module::net::utils
{
    template <class Exception>
    std::exception_ptr makeError()
    {
        return std::make_exception_ptr(Exception{});
    }

    template <class Exception>
    std::exception_ptr makeError(std::string&& what)
    {
        return std::make_exception_ptr(Exception{std::move(what)});
    }

    template <class FutureT, class Exception>
    cmt::Future<FutureT> makeError(std::string&& what)
    {
        return cmt::readyFuture<FutureT>(makeError<Exception>(std::move(what)));
    }

    inline std::exception_ptr makeError(const std::error_code& ec)
    {
#ifdef _WIN32
#   define ECODE(v) WSA##v
#else
#   define ECODE(v) v
#endif
        switch(ec.value())
        {
        case ECODE(EADDRINUSE)     : return makeError<api::AddressInIse>();
        case ECODE(EADDRNOTAVAIL)  : return makeError<api::AddressNotAvailable>();
        case ECODE(EISCONN)        : return makeError<api::AlreadyConnected>();
        case ECODE(EFAULT)         : return makeError<api::BadAddress>();
        case ECODE(ECONNABORTED)   : return makeError<api::ConnectionAborted>();
        case ECODE(ECONNREFUSED)   : return makeError<api::ConnectionRefused>();
        case ECODE(ECONNRESET)     : return makeError<api::ConnectionReset>();
        case ECODE(EHOSTUNREACH)   : return makeError<api::HostUnreachable>();
        case ECODE(EINVAL)         : return makeError<api::InvalidArgument>();
        case ECODE(ENETDOWN)       : return makeError<api::NetworkDown>();
        case ECODE(ENETRESET)      : return makeError<api::NetworkReset>();
        case ECODE(ENETUNREACH)    : return makeError<api::NetworkUnreachable>();
        case ECODE(ENOTCONN)       : return makeError<api::NotConnected>();
        case ECODE(EINPROGRESS)    : return makeError<api::OperationInProgress>();
        case ECODE(EALREADY)       : return makeError<api::OperationInProgress>();
        case ECODE(EOPNOTSUPP)     : return makeError<api::OperationNotSupported>();
        case ECODE(EACCES)         : return makeError<api::PermissionDenied>();
        case ECODE(ETIMEDOUT)      : return makeError<api::TimedOut>();
#ifdef _WIN32
        case WSAECANCELLED         : return makeError<api::OperationCanceled>();
        case WSATRY_AGAIN          : return makeError<api::UnavaliableTryAgain>();
        case WSAEWOULDBLOCK        : return makeError<api::UnavaliableTryAgain>();
#else
        case ECANCELED              : return makeError<api::OperationCanceled>();
        case EPERM                  : return makeError<api::PermissionDenied>();
#   if EAGAIN!=EWOULDBLOCK
        case EAGAIN                 : return makeError<api::UnavaliableTryAgain>();
#   endif
        case EWOULDBLOCK            : return makeError<api::UnavaliableTryAgain>();
#endif
        default: break;
        }
#undef ECODE

        return makeError<api::Error>(ec.message());
    }

    template <class FutureT>
    cmt::Future<FutureT> makeError(const std::error_code& ec)
    {
        return cmt::readyFuture<FutureT>(makeError(ec));
    }

    inline std::error_code fetchSystemErrorCode()
    {
#ifdef _WIN32
        return dci::utils::win32::error::make(WSAGetLastError());
#else
        return std::error_code{errno, std::generic_category()};
#endif
    }

    inline std::exception_ptr fetchSystemError()
    {
        return makeError(fetchSystemErrorCode());
    }

    template <class FutureT>
    cmt::Future<FutureT> fetchSystemError()
    {
        return cmt::readyFuture<FutureT>(fetchSystemError());
    }
}
