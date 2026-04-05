/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "epoll.hpp"
#include <dci/poll/descriptor.hpp>
#include <dci/poll/error.hpp>

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

namespace dci::poll::impl::polling
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Epoll::Epoll()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Epoll::~Epoll()
    {
        wakeup();
	deinitialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Epoll::initialize()
    {
        if(-1 != _fd)
        {
            return error::already_initialized;
        }

        _fd = epoll_create1(EPOLL_CLOEXEC);
        if(-1 == _fd)
        {
            return std::error_code{errno, std::generic_category()};
        }

        _wakeupEvent = eventfd(1, EFD_NONBLOCK|EFD_CLOEXEC);

        if(0 > _wakeupEvent)
        {
            int err = errno;
            deinitialize();
            return std::error_code(err, std::generic_category());
        }

        epoll_event evt{0,{nullptr}};
        evt.data.fd = _wakeupEvent;
        evt.events = EPOLLIN | EPOLLOUT | EPOLLET;

        if(epoll_ctl(_fd, EPOLL_CTL_ADD, _wakeupEvent, &evt))
        {
            int err = errno;
            deinitialize();
            return std::error_code(err, std::generic_category());
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Epoll::initialized() const
    {
        return -1 != _fd;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Epoll::installDescriptor(Descriptor* d)
    {
        epoll_event evt{0,{nullptr}};
        evt.data.ptr = d;
        evt.events = EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLHUP | EPOLLRDHUP | EPOLLET;

        if(epoll_ctl(_fd, EPOLL_CTL_ADD, d->native(), &evt))
        {
            return std::error_code{errno, std::generic_category()};
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Epoll::uninstallDescriptor(Descriptor* d)
    {
        if(epoll_ctl(_fd, EPOLL_CTL_DEL, d->native(), nullptr))
        {
            return std::error_code{errno, std::generic_category()};
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Epoll::execute(clocking::Duration timeout)
    {
        if(timeout.count() < 0)
        {
            timeout = clocking::Duration{};
        }

        epoll_event* eventsBuffer = reinterpret_cast<epoll_event *>(_eventsBuffer);
#if HAVE_EPOLL_PWAIT2
            struct timespec timespec;
            timespec.tv_sec = std::chrono::floor<std::chrono::seconds>(timeout).count();
            timespec.tv_nsec = std::chrono::floor<std::chrono::nanoseconds>(timeout - std::chrono::seconds{timespec.tv_sec}).count();
            int eventsAmount = epoll_pwait2(
                                   _fd,
                                   eventsBuffer,
                                   sizeof(_eventsBuffer)/sizeof(epoll_event),
                                   &timespec,
                                   nullptr);
#else
            int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
            int eventsAmount = epoll_pwait(
                                   _fd,
                                   eventsBuffer,
                                   sizeof(_eventsBuffer)/sizeof(epoll_event),
                                   milliseconds,
                                   nullptr);
#endif

        if(-1 == eventsAmount)
        {
            if(-1 == _fd)
            {
                return error::not_initialized;
            }

            return std::error_code{errno, std::generic_category()};
        }

        for(int idx(0); idx<eventsAmount; ++idx)
        {
            epoll_event& evt = eventsBuffer[idx];

            using RSF = descriptor::ReadyStateFlags;
            RSF readyState =
                    ((evt.events & (EPOLLIN))               ? RSF::rsf_read  : RSF::rsf_null) |
                    ((evt.events & (EPOLLPRI))              ? RSF::rsf_pri   : RSF::rsf_null) |
                    ((evt.events & (EPOLLOUT))              ? RSF::rsf_write : RSF::rsf_null) |
                    ((evt.events & (EPOLLERR))              ? RSF::rsf_error : RSF::rsf_null) |
                    ((evt.events & (EPOLLHUP|EPOLLRDHUP))   ? RSF::rsf_eof   : RSF::rsf_null) |
                    RSF::rsf_null;

            if(_wakeupEvent == evt.data.fd)
            {
                if(RSF::rsf_read & readyState)
                {
                    eventfd_t value;
                    eventfd_read(_wakeupEvent, &value);
                }
            }
            else
            {
                Descriptor* d = static_cast<Descriptor *>(evt.data.ptr);
                d->setReadyState(readyState);
            }
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Epoll::wakeup()
    {
        if(-1 == _fd)
        {
            return error::not_initialized;
        }

        int res = eventfd_write(_wakeupEvent, 1);

        if(0 > res)
        {
            return std::error_code{errno, std::generic_category()};
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Epoll::deinitialize()
    {
        if(-1 == _fd)
        {
            return error::not_initialized;
        }

        {
            int closeRes = ::close(_wakeupEvent);
            while(0 != closeRes && EINTR == errno)
            {
                //try again
                closeRes = ::close(_wakeupEvent);
            }
            _wakeupEvent = -1;
        }

        std::error_code ec;

        int closeRes = ::close(_fd);
        while(0 != closeRes && EINTR == errno)
        {
            //try again
            closeRes = ::close(_fd);
        }

        if(closeRes)
        {
            ec = std::error_code{errno, std::generic_category()};
        }

        _fd = -1;

        return ec;
    }

}
