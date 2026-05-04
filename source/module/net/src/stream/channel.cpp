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

namespace dci::module::net::stream
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(Host* host, poll::descriptor::Native sock, const api::Endpoint& localEndpoint, api::Endpoint&& remoteEndpoint)
        : api::stream::Channel<>::Opposite(idl::interface::Initializer())
        , _host{host}
        , _sock{sock}
        , _localEndpoint{localEndpoint}
        , _remoteEndpoint{std::move(remoteEndpoint)}
        , _connectPromise{cmt::PromiseNullInitializer{}}
        , _connected{_sock.valid()}
    {
        if(_connected)
        {
            _sock.ready() += _sockReadyOwner * [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState){connectedSockReady(native, readyState);};
        }

        _host->track(this);

        methods()->setOption() += this * [&](const api::Option& op)
        {
            if(_connected)
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

        methods()->localEndpoint() += this * [&]()
        {
            return cmt::readyFuture(_localEndpoint);
        };

        methods()->remoteEndpoint() += this * [&]()
        {
            return cmt::readyFuture(_remoteEndpoint);
        };

        methods()->send() += this * [&](auto&& bytes)
        {
            if(!_connected)
            {
                failed(utils::makeError<api::NotConnected>());
                return;
            }
            _sendBuffer.push(std::forward<decltype(bytes)>(bytes));
            if(poll::descriptor::rsf_write & _lastReadyState)
            {
                doWrite(_sock.native());
            }
        };

        methods()->setReceiveGranula() += this * [&](uint64 granula)
        {
            setReceiveGranula(granula);
        };

        methods()->startReceive() += this * [&]()
        {
            setReceiveGranula(std::numeric_limits<uint32>::max());
        };

        methods()->stopReceive() += this * [&]()
        {
            setReceiveGranula(0);
        };

        methods()->shutdown() += this * [&](bool input, bool output)
        {
            shutdown(input, output);
        };

        methods()->close() += this * [&]()
        {
            close();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        *_aliveMarker = false;
        sbs::Owner::flush();
        _sockReadyOwner.flush();
        _sock.close();
        _host->untrack(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<api::stream::Channel<>> Channel::connect(bool needBind)
    {
        dbgAssert(!_connected);
        dbgAssert(!_sock.valid());
        dbgAssert(!_connectPromise.charged());

#ifdef _WIN32
        dci::poll::descriptor::Native native = ::WSASocketW(utils::sockaddr::family(_remoteEndpoint), SOCK_STREAM, PF_UNSPEC, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
#else
        dci::poll::descriptor::Native native = ::socket(utils::sockaddr::family(_remoteEndpoint), SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
#endif

        if(native._bad == native._value)
        {
            return utils::fetchSystemError<api::stream::Channel<>>();
        }

        std::error_code ec = _sock.attach(native);
        if(ec)
        {
            return utils::makeError<api::stream::Channel<>>(ec);
        }

        ExceptionPtr e = applyOptions(_sock);
        if(e)
        {
            return cmt::readyFuture<api::stream::Channel<>>(e);
        }

        if(needBind)
        {
            union
            {
                sockaddr            _base {};
                sockaddr_storage    _space;
            } saddr;

            socklen_t saddrLen = utils::sockaddr::convert(_localEndpoint, &saddr._base);
            if(::bind(native, &saddr._base, saddrLen))
            {
                return utils::fetchSystemError<api::stream::Channel<>>();
            }
        }

        union
        {
            sockaddr            _base;
            sockaddr_storage    _space;
        } saddr;

        socklen_t saddrLen = utils::sockaddr::convert(_remoteEndpoint, &saddr._base);
        int res = ::connect(native, &saddr._base, saddrLen);

        if(!res)
        {
            _connected = true;
            _sockReadyOwner.flush();
            _sock.ready() += _sockReadyOwner * [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState){connectedSockReady(native, readyState);};
            return cmt::readyFuture(api::stream::Channel<>(*this));
        }

#ifdef _WIN32
        bool failed = WSAEWOULDBLOCK != WSAGetLastError();
#else
        bool failed = EINPROGRESS != errno;
#endif
        if(failed)
        {
            return utils::fetchSystemError<api::stream::Channel<>>();
        }

        _connectPromise = cmt::Promise<api::stream::Channel<>>();

        _connectPromise.canceled() += [this]
        {
            _connectPromise.uncharge();
            _sockReadyOwner.flush();
            _sock.close();
        };

        _sockReadyOwner.flush();
        _sock.ready() += _sockReadyOwner * [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState){connectSockReady(native, readyState);};
        return _connectPromise.future();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::setReceiveGranula(uint64 granula)
    {
        if(granula > std::numeric_limits<uint32>::max())
        {
            failed(utils::makeError<api::InvalidArgument>());
            return;
        }

        uint64 prevReceiveGranula = _receiveGranula;
        _receiveGranula = granula;

        if(_connected && !prevReceiveGranula && _receiveGranula && (poll::descriptor::rsf_read & _lastReadyState))
        {
            doRead(_sock.native());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::failed(ExceptionPtr e, bool doClose)
    {
        api::stream::Channel<>::Opposite si = *this;
        bool emitClosed = false;

        AliveMarker aliveMarker{_aliveMarker};

        if(doClose)
        {
            if(_connected)
            {
                emitClosed = true;
                _connected = false;
            }

            if(_connectPromise.charged() && !_connectPromise.resolved())
            {
                _connectPromise.resolveException(e);
                if(!*aliveMarker)
                    return;
            }

            if(!(poll::descriptor::rsf_close & _lastReadyState))
            {
                _sockReadyOwner.flush();
                _sock.close();
            }

            _sendBuffer.clear();
        }

        si->failed(e);
        if(!*aliveMarker)
            return;

        if(emitClosed)
        {
            si->closed();
            if(!*aliveMarker)
                return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::shutdown(bool input, bool output)
    {
        if(!_sock.valid())
            return;

        if(input)
            _receiveGranula = 0;

        if(output && _connected && (poll::descriptor::rsf_write & _lastReadyState))
        {
            AliveMarker aliveMarker{_aliveMarker};
            doWrite(_sock.native(), true);
            if(!*aliveMarker)
                return;
        }

        _sock.shutdown(input, output);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::close()
    {
        AliveMarker aliveMarker{_aliveMarker};

        if(_connectPromise.charged() && !_connectPromise.resolved())
        {
            _connectPromise.resolveCancel();
            if(!*aliveMarker)
                return;
        }

        if(_connected && (poll::descriptor::rsf_write & _lastReadyState))
        {
            doWrite(_sock.native(), true);
            if(!*aliveMarker)
                return;
        }

        if(!(poll::descriptor::rsf_close & _lastReadyState))
        {
            _sockReadyOwner.flush();
            _sock.close();
        }

        _lastReadyState = poll::descriptor::rsf_close;
        _sendBuffer.clear();

        if(_connected)
        {
            _connected = false;
            methods()->closed();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Channel::doWrite(poll::descriptor::Native native, bool preCloseMode)
    {
        if(_sock.native()._value == poll::descriptor::Native::_bad)
            return false;

        dbgAssert(_sock.native() == native);
        dbgAssert(_lastReadyState & poll::descriptor::rsf_write);

        if(_sendBuffer.empty())
        {
            return false;
        }

        uint32 totalWrote = 0;

        while((poll::descriptor::rsf_write & _lastReadyState) && !_sendBuffer.empty())
        {
#ifdef _WIN32
            DWORD sent{};
            ssize_t res = ::WSASend(native, _sendBuffer.bufs(), _sendBuffer.bufsAmount(), &sent, 0, nullptr, nullptr);
            if(!res)
            {
                res = sent;
            }
#else
            msghdr msg = {nullptr, 0, _sendBuffer.bufs(), _sendBuffer.bufsAmount(), nullptr, 0, 0};
            ssize_t res = ::sendmsg(native, &msg, MSG_NOSIGNAL);
#endif

            if(0 > res)
            {
#ifdef _WIN32
                DWORD lastError = WSAGetLastError();
                bool noSpaceInSocketYet = (WSATRY_AGAIN == lastError) || (WSAEWOULDBLOCK == lastError);
#else
                bool noSpaceInSocketYet = EAGAIN == errno;
#endif
                if(noSpaceInSocketYet)
                {
                    _lastReadyState &= ~poll::descriptor::rsf_write;
                }
                else
                {
                    if(preCloseMode)
                    {
                        _sendBuffer.clear();
                    }
                    else
                    {
                        failed(utils::fetchSystemError(), true);
                    }
                    return false;
                }

                break;
            }

            uint32 wrote = static_cast<uint32>(res);
            if(wrote)
            {
                dbgAssert(wrote <= _sendBuffer.bufsSize());

                totalWrote += wrote;
                _sendBuffer.drop(wrote);
            }
        }

        if(!preCloseMode && totalWrote)
        {
            uint32 stillWait = _sendBuffer.dataSize();
            methods()->sended(totalWrote, stillWait);
        }

        return 0 < totalWrote;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Channel::doRead(poll::descriptor::Native native)
    {
        if(_sock.native()._value == poll::descriptor::Native::_bad)
            return false;

        dbgAssert(_sock.native() == native);
        dbgAssert(_lastReadyState & poll::descriptor::rsf_read);

        utils::RecvBuffer* recvBuffer = _host->getRecvBuffer();
        uint32 totalReaded = 0;
        while((poll::descriptor::rsf_read & _lastReadyState) && _receiveGranula)
        {
            recvBuffer->limitDataSize(_receiveGranula);

#ifdef _WIN32
            DWORD received{};
            DWORD flags{};
            ssize_t res = ::WSARecv(native, recvBuffer->bufs(), recvBuffer->bufsAmount(), &received, &flags, nullptr, nullptr);
            if(!res)
            {
                res = received;
            }
#else
            ssize_t res = ::readv(native, recvBuffer->bufs(), static_cast<int>(recvBuffer->bufsAmount()));
#endif

            recvBuffer->unlimitDataSize();

            if(0 > res)
            {

#ifdef _WIN32
                DWORD lastError = WSAGetLastError();
                bool noDataInSocketYet = (WSATRY_AGAIN == lastError) || (WSAEWOULDBLOCK == lastError);
#else
                bool noDataInSocketYet = EAGAIN == errno;
#endif
                if(noDataInSocketYet)
                {
                    _lastReadyState &= ~poll::descriptor::rsf_read;
                    break;
                }

                if(!(_lastReadyState & poll::descriptor::rsf_eof))
                {
                    _lastReadyState |= ~poll::descriptor::rsf_error;
                    failed(utils::fetchSystemError(), true);
                }
                return false;
            }
            else if(0 == res)
            {
                //peer closed
                _lastReadyState &= ~poll::descriptor::rsf_read;
                _lastReadyState |= ~poll::descriptor::rsf_eof;
                return false;
            }

            uint32 readed = static_cast<uint32>(res);
            totalReaded += readed;
            methods()->received(recvBuffer->detach(readed));
        }

        return 0 < totalReaded;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::connectSockReady(poll::descriptor::Native /*native*/, poll::descriptor::ReadyStateFlags readyState)
    {
        dbgAssert(!_connected);

        dbgAssert(_connectPromise.charged());
        dbgAssert(!_connectPromise.resolved());

        auto connectPromise = std::exchange(_connectPromise, cmt::Promise<api::stream::Channel<>>(cmt::PromiseNullInitializer()));

        if(poll::descriptor::rsf_error & readyState)
        {
            std::error_code ec = _sock.error();
            _sockReadyOwner.flush();
            _sock.close();

            connectPromise.resolveException(utils::makeError(ec));
            return;
        }

        if((poll::descriptor::rsf_eof|poll::descriptor::rsf_close) & readyState)
        {
            _sockReadyOwner.flush();
            _sock.close();

            connectPromise.resolveException(exception::buildInstance<api::ConnectionClosed>());
            return;
        }

        if((poll::descriptor::rsf_read|poll::descriptor::rsf_write) & readyState)
        {
            _connected = true;
            _lastReadyState |= readyState;
            _sockReadyOwner.flush();
            _sock.ready() += _sockReadyOwner * [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState){connectedSockReady(native, readyState);};

            connectPromise.resolveValue(api::stream::Channel<>(*this));
            return;
        }

        dbgWarn("never here");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::connectedSockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState)
    {
        dbgAssert(_connected);
        AliveMarker aliveMarker{_aliveMarker};

        _lastReadyState |= readyState;

        bool someProcessed = true;
        while(someProcessed)
        {
            someProcessed = false;

            if(poll::descriptor::rsf_error & _lastReadyState)
            {
                _lastReadyState = {};

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

            if(poll::descriptor::rsf_write & _lastReadyState)
            {
                someProcessed |= doWrite(native);
                if(!*aliveMarker)
                    return;
            }

            if(poll::descriptor::rsf_read & _lastReadyState)
            {
                someProcessed |= doRead(native);
                if(!*aliveMarker)
                    return;
            }

            if(poll::descriptor::rsf_eof & _lastReadyState)
            {
                _lastReadyState = {};

                close();
                return;
            }
        }
    }
}
