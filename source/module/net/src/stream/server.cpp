// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "server.hpp"
#include "../host.hpp"
#include "../utils/sockaddr.hpp"
#include "../utils/makeError.hpp"
#include "dci/poll/descriptor/native.hpp"

namespace dci::module::net::stream
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Server::Server(Host* host)
        : api::stream::Server<>::Opposite(idl::interface::Initializer())
        , _host{host}
        , _sock{poll::descriptor::Native{}, [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState) {sockReady(native, readyState);}}
    {
        _host->track(this);

        methods()->setOption() += this * [&](const api::Option& op)
        {
            if(_sock.valid())
            {
                ExceptionPtr e = applyOption(_sock.native(), op);
                if(e)
                {
                    return cmt::readyFuture<None>(e);
                }

                return cmt::readyFuture(None{});
            }

            pushOption(op);
            return cmt::readyFuture(None{});
        };

        methods()->listen() += this * [&](auto&& endpoint)
        {
            return listen(std::forward<decltype(endpoint)>(endpoint));
        };

        methods()->localEndpoint() += this * [&]()
        {
            return cmt::readyFuture(_localEndpoint);
        };

        methods()->close() += this * [&]()
        {
            close();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Server::~Server()
    {
        sbs::Owner::flush();
        _host->untrack(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<None> Server::listen(auto&& endpoint)
    {
        close();

        _bindEndpoint = std::forward<decltype(endpoint)>(endpoint);

        union
        {
            sockaddr            _base;
            sockaddr_un         _un;
            sockaddr_storage    _space;
        } saddr;
        socklen_t saddrLen = utils::sockaddr::convert(_bindEndpoint, &saddr._base);

#ifdef _WIN32
        dci::poll::descriptor::Native native = ::WSASocketW(saddr._base.sa_family, SOCK_STREAM, PF_UNSPEC, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
#else
        poll::descriptor::Native native = ::socket(saddr._base.sa_family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
#endif

        if(native._bad == native._value)
        {
            return utils::fetchSystemError<None>();
        }

        std::error_code ec = _sock.attach(native);
        if(ec)
        {
            return utils::makeError<None>(ec);
        }

        ExceptionPtr e = applyOptions(_sock.native());
        if(e)
        {
            return cmt::readyFuture<None>(e);
        }

        if(AF_UNIX == saddr._base.sa_family)
        {
            unlink(saddr._un.sun_path);
        }

        if(::bind(native, &saddr._base, saddrLen))
        {
            return utils::fetchSystemError<None>();
        }

        saddrLen = sizeof(saddr);
        if(::getsockname(native, &saddr._base, &saddrLen))
        {
            return utils::fetchSystemError<None>();
        }
        utils::sockaddr::convert(&saddr._base, saddrLen, _localEndpoint);

        if(::listen(native, 5))
        {
            return utils::fetchSystemError<None>();
        }

        return cmt::readyFuture(None{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Server::close()
    {
        if(api::LocalEndpoint * l = std::get_if<api::LocalEndpoint>(&_bindEndpoint.std()))
        {
            unlink(l->address.data());
        }
        _sock.close();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Server::sockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState)
    {
        if(poll::descriptor::rsf_error & readyState)
        {
            std::error_code ec = _sock.error();
            if(ec)
            {
                methods()->failed(utils::makeError(ec));
            }

            close();
            return;
        }

        if(poll::descriptor::rsf_eof & readyState)
        {
            methods()->closed();
            _bindEndpoint = api::Endpoint();
            _localEndpoint = api::Endpoint();
            return;
        }

        if(poll::descriptor::rsf_read & readyState)
        {
            for(;;)
            {
                union
                {
                    sockaddr            _base;
                    sockaddr_storage    _space;
                } saddr;
                ::socklen_t saddrLen = sizeof(saddr);

#ifdef _WIN32
                poll::descriptor::Native native2 = ::accept(native, &saddr._base, &saddrLen);
#else
                poll::descriptor::Native native2 = ::accept4(native, &saddr._base, &saddrLen, SOCK_NONBLOCK|SOCK_CLOEXEC);
#endif
                if(native2._bad != native2._value)
                {
                    api::Endpoint remoteEndpoint;
                    utils::sockaddr::convert(&saddr._base, saddrLen, remoteEndpoint);

                    stream::Channel* c = new stream::Channel{_host, native2, _localEndpoint, std::move(remoteEndpoint)};
                    c->involvedChanged() += c * [c](bool v)
                    {
                        if(!v)
                        {
                            delete c;
                        }
                    };
                    methods()->accepted(api::stream::Channel<>(*c));
                }
                else
                {
#ifdef _WIN32
                    bool failed = WSAEWOULDBLOCK != WSAGetLastError();
#else
                    bool failed = EAGAIN != errno;
#endif
                    if(failed)
                    {
                        methods()->failed(utils::fetchSystemError());
                    }
                    break;
                }
            }
        }
    }
}
