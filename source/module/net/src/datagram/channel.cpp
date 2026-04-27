/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "channel.hpp"
#include "../host.hpp"
#include "../utils/sockaddr.hpp"
#include "../utils/makeError.hpp"
#include "dci/poll/descriptor/native.hpp"
#include "sendBuffer.hpp"

namespace dci::module::net::datagram
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(Host * host)
        : api::datagram::Channel<>::Opposite{idl::interface::Initializer{}}
        , _host{host}
        , _sock{{}, [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState){sockReady(native, readyState);}}
    {
        _host->track(this);

        methods()->setOption() += this * [&](const api::Option& op)
        {
            if(_opened)
            {
                ExceptionPtr e = applyOption(_sock, op);
                if(e)
                {
                    return cmt::readyFuture<None>(e);
                }

                return cmt::readyFuture(None{});
            }

            pushOption(op);
            return cmt::readyFuture(None{});
        };

        methods()->bind() += this * [&](const api::Endpoint& ep)
        {
            _sock.close();

            ExceptionPtr e = open(&ep);
            if(e)
            {
                return cmt::readyFuture<None>(e);
            }

            return cmt::readyFuture(None{});
        };

        methods()->localEndpoint() += this * [&]()
        {
            if(!_opened)
            {
                return utils::makeError<api::Endpoint, api::Error>("not initialized");
            }

            if(!_localEndpointFetched)
            {
                union
                {
                    sockaddr            _base;
                    sockaddr_storage    _space;
                } saddr;
                socklen_t saddrLen = sizeof(saddr);
                if(::getsockname(_sock.native(), &saddr._base, &saddrLen))
                {
                    return utils::fetchSystemError<api::Endpoint>();
                }
                utils::sockaddr::convert(&saddr._base, saddrLen, _localEndpoint);

                _localEndpointFetched = true;
            }

            return cmt::readyFuture(_localEndpoint);
        };

        methods()->send() += this * [&](const Bytes& data, const api::Endpoint& peer)
        {
            if(!_opened)
            {
                ExceptionPtr e = open(nullptr, &peer);
                if(e)
                {
                    failed(e);
                    return;
                }
            }

            doSend(_sock.native(), std::forward<decltype(data)>(data), peer);
        };

        methods()->close() += this * [&]()
        {
            close();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        sbs::Owner::flush();
        _sock.close();
        _host->untrack(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::failed(ExceptionPtr e, bool doClose)
    {
        api::datagram::Channel<>::Opposite si = *this;
        bool emitClosed = false;

        if(doClose)
        {
            if(_opened)
            {
                emitClosed = true;
                _opened = false;
            }

            _sock.close();
        }

        si->failed(e);

        if(emitClosed)
        {
            si->closed();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::close()
    {
        _sock.close();

        if(_opened)
        {
            _opened = false;
            methods()->closed();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionPtr Channel::open(const api::Endpoint* bind, const api::Endpoint* peer)
    {
        dbgAssert(!_sock.valid());
        dbgAssert(!_opened);
        dbgAssert(bind || peer);

#ifdef _WIN32
        dci::poll::descriptor::Native native = ::WSASocketW(utils::sockaddr::family(bind ? *bind : *peer), SOCK_DGRAM, PF_UNSPEC, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
#else
        dci::poll::descriptor::Native native = ::socket(utils::sockaddr::family(bind ? *bind : *peer), SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC, PF_UNSPEC);
#endif
        if(native._bad == native._value)
        {
            return utils::fetchSystemError();
        }

        std::error_code ec = _sock.attach(native);
        if(ec)
        {
            _sock.close();
            return utils::makeError(ec);
        }

        ExceptionPtr e = applyOptions(_sock);
        if(e)
        {
            _sock.close();
            return e;
        }

        if(bind)
        {
            union
            {
                sockaddr            _base;
                sockaddr_storage    _space;
            } saddr;
            socklen_t saddrLen = utils::sockaddr::convert(*bind, &saddr._base);

            if(::bind(native, &saddr._base, saddrLen))
            {
                std::exception_ptr e = utils::fetchSystemError();
                _sock.close();
                return e;
            }
        }

        _opened = true;
        return ExceptionPtr{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::doSend(poll::descriptor::Native native, const Bytes& data, const api::Endpoint& peer)
    {
        if(data.empty())
        {
            return;
        }

        union
        {
            sockaddr            _base;
            sockaddr_storage    _space;
        } saddr;
        socklen_t saddrLen = utils::sockaddr::convert(peer, &saddr._base);

        SendBuffer* sendBuffer = _host->getDatagramSendBuffer();

#ifdef _WIN32
#else
        msghdr msg = {&saddr._base, saddrLen, sendBuffer->bufs(), 0, nullptr, 0, 0};
#endif

        bytes::Cursor src = data.begin();
        while(SendBuffer::SourceUtilization::partial == sendBuffer->fillFrom(src))
        {
#ifdef _WIN32
            DWORD sent{};
            ssize_t res = ::WSASendTo(native, sendBuffer->bufs(), sendBuffer->bufsAmount(), &sent, MSG_PARTIAL, &saddr._base, saddrLen, nullptr, nullptr);
            if(!res)
            {
                res = sent;
            }
#else
            msg.msg_iovlen = sendBuffer->bufsAmount();
            ssize_t res = ::sendmsg(native, &msg, MSG_MORE);
#endif
            sendBuffer->clear();

            if(0 > res)
            {
                std::error_code ec = utils::fetchSystemErrorCode();
                failed(utils::makeError(ec), ec != std::errc::resource_unavailable_try_again);
                return;
            }
        }

#ifdef _WIN32
        dbgAssert(sendBuffer->bufsAmount());
        DWORD sent{};
        ssize_t res = ::WSASendTo(native, sendBuffer->bufs(), sendBuffer->bufsAmount(), &sent, 0, &saddr._base, saddrLen, nullptr, nullptr);
        if(!res)
        {
            res = sent;
        }
#else
        msg.msg_iovlen = sendBuffer->bufsAmount();
        dbgAssert(msg.msg_iovlen);
        ssize_t res = ::sendmsg(native, &msg, 0);
#endif
        sendBuffer->clear();

        if(0 > res)
        {
            std::error_code ec = utils::fetchSystemErrorCode();
            failed(utils::makeError(ec), ec != std::errc::resource_unavailable_try_again);
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::doRecv(poll::descriptor::Native native)
    {
        union
        {
            sockaddr            _base;
            sockaddr_storage    _space;
        } saddr;
        socklen_t saddrLen = sizeof(saddr);

        auto recvBuffer = _host->getRecvBuffer();
#ifdef _WIN32
        DWORD received{};
        DWORD flags{};
        ssize_t res = ::WSARecvFrom(native, recvBuffer->bufs(), recvBuffer->bufsAmount(), &received, &flags, &saddr._base, &saddrLen, nullptr, nullptr);
        if(!res)
        {
            res = received;
        }
#else
        msghdr msg = {&saddr._base, saddrLen, recvBuffer->bufs(), recvBuffer->bufsAmount(), nullptr, 0, 0};
        ssize_t res = ::recvmsg(native, &msg, 0);
        saddrLen = msg.msg_namelen;
#endif
        if(0 > res)
        {
            failed(utils::fetchSystemError(), true);
            return;
        }

        api::Endpoint peer;
        utils::sockaddr::convert(&saddr._base, saddrLen, peer);

        methods()->received(recvBuffer->detach(static_cast<uint32>(res)), peer);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::sockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState)
    {
        if(poll::descriptor::rsf_error & readyState)
        {
            std::error_code ec = _sock.error();
            if(ec)
            {
                failed(utils::makeError(ec), true);
            }
            else
            {
                close();
            }

            return;
        }

        if(poll::descriptor::rsf_eof & readyState)
        {
            _sock.close();
            return;
        }

        if(poll::descriptor::rsf_close & readyState)
        {
            _opened = false;
            _localEndpoint = api::Endpoint();
            _localEndpointFetched = false;
            _sock.close();
            methods()->closed();
            return;
        }

        if(poll::descriptor::rsf_read & readyState)
        {
            doRecv(native);
        }
    }
}
