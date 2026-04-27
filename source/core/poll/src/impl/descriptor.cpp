/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#if __has_include(<Winsock2.h>)
#   include <Winsock2.h>
#endif

#include "descriptor.hpp"
#include "service.hpp"
#include <dci/poll/descriptor.hpp>
#include <dci/logger.hpp>
#include <dci/cmt/functions.hpp>
#include <dci/utils/atScopeExit.hpp>

#ifdef _WIN32
#   include <dci/utils/win32/error.hpp>
#endif

#include <unistd.h>
#include <sys/types.h>

#if __has_include(<sys/socket.h>)
#   include <sys/socket.h>
#endif

namespace dci::poll::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::Descriptor(Native native, cmt::Raisable* raisable)
        : _native{native}
        , _readyRaisable{raisable}
    {
        if(valid())
        {
            std::error_code ec = install();
            if(ec)
            {
                setReadyState(descriptor::rsf_error);
                close();
            }
        }

        if(_readyState && _readyRaisable)
        {
            _readyRaisable->raise();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::~Descriptor()
    {
        close();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, descriptor::Native /*native*/, descriptor::ReadyStateFlags /*readyState*/> Descriptor::ready()
    {
        return _readyWire.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::emitReady()
    {
        if(_readyState)
        {
            if(_readyWire.connected())
            {
                ReadyStateFlags readyState = std::exchange(_readyState, {});
                _readyWire.in(_native, readyState);
            }
            else if(_readyRaisable)
            {
                _readyRaisable->raise();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::setRaisable(cmt::Raisable* raisable)
    {
        _readyRaisable = raisable;
        dbgAssert(_readyRaisable);

        if(_readyState && _readyRaisable)
        {
            _readyRaisable->raise();
        }
    }

    void Descriptor::resetRaisable()
    {
        _readyRaisable = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Descriptor::valid() const
    {
#ifdef _WIN32
        return _native._value != _native._bad && !!_native._value;
#else
        return _native._value >= 0;
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::error()
    {
#ifdef _WIN32
        int errcode = ENOTSOCK;
        int errcodelen = sizeof(errcode);
        if(SOCKET_ERROR == getsockopt(_native, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&errcode), &errcodelen))
        {
            errcode = WSAGetLastError();
        }

        return utils::win32::error::make(errcode);
#else
        int errcode = ENOTSOCK;
        socklen_t errcodelen = sizeof(errcode);
        if(-1 == getsockopt(_native, SOL_SOCKET, SO_ERROR, &errcode, &errcodelen))
        {
            errcode = errno;
        }

        return std::error_code{errcode, std::generic_category()};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    descriptor::Native Descriptor::native() const
    {
        return _native;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::shutdown(bool input, bool output)
    {
        std::error_code ec;
        if(valid() && (input | output))
        {
            int how;
#ifdef WIN32
#   define SHUT_RDWR SD_BOTH
#   define SHUT_RD SD_RECEIVE
#   define SHUT_WR SD_SEND
#endif
            if(input && output)
                how = SHUT_RDWR;
            else if(input)
                how = SHUT_RD;
            else
                how = SHUT_WR;

            if(0 != ::shutdown(_native, how))
            {
                ec = std::error_code{errno, std::generic_category()};
            }
        }

        return ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::close(bool withUninstall)
    {
        std::error_code ec;
        if(valid())
        {
            if(withUninstall)
            {
                ec = uninstall();
            }

#ifdef _WIN32
            if(int closeRes = closesocket(_native))
            {
                ec = utils::win32::error::make(closeRes);
            }
#else
            int res = ::close(_native);
            if(res)
            {
                ec = std::error_code{errno, std::generic_category()};
            }
#endif
            _native = {};
            _readyState = _readyState & ~(descriptor::rsf_read | descriptor::rsf_pri | descriptor::rsf_write);

            setReadyState(descriptor::rsf_close | descriptor::rsf_eof);
        }

        return ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::attach(Native native)
    {
        close();

        _native = native;
        _readyState = {};

        if(valid())
        {
            std::error_code ec = install();
            if(ec)
            {
                setReadyState(descriptor::rsf_error);
                close();
                return ec;
            }
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::detach()
    {
        std::error_code ec;

        if(valid())
        {
            ec = uninstall();
            _native = {};
            _readyState = {};
        }

        return ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    descriptor::ReadyStateFlags Descriptor::readyState() const
    {
        return _readyState;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::resetReadyState(ReadyStateFlags flags)
    {
        _readyState &= ~flags;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::setReadyState(ReadyStateFlags flags)
    {
        _readyState |= flags;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::install()
    {
        dbgAssert(valid());

        return service.polling().installDescriptor(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::uninstall()
    {
        dbgAssert(valid());

        return service.polling().uninstallDescriptor(this);
    }
}
