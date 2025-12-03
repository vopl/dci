// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "acceptor.hpp"
#include "channel.hpp"
#include "address2Endpoint.hpp"
#include "endpoint2Address.hpp"

namespace dci::module::ppn::transport::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::Acceptor(host::Manager* hostManager)
        : apit::net::Acceptor<>::Opposite(idl::interface::Initializer())
        , _hostManager(hostManager)
    {
        //in address() -> transport::Address;
        methods()->address() += serviceSol() * [this]
        {
            return cmt::readyFuture(_boundAddress);
        };

        //in cost() -> real64;
        methods()->cost() += serviceSol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in rtt() -> real64;
        methods()->rtt() += serviceSol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in bandwidth() -> real64;
        methods()->bandwidth() += serviceSol() * []
        {
            return cmt::readyFuture(std::numeric_limits<real64>::max());
        };

        //in bind(Address) -> void;
        methods()->bind() += serviceSol() * [this](apit::Address&& address)
        {
            if(_started)
            {
                return cmt::readyFuture<None>(exception::buildInstance<api::AlreadyBound>("unable to bind after acceptor started"));
            }

            auto scheme = utils::uri::scheme(address.value);
            using namespace std::literals;
            if("local"sv != scheme &&
               "tcp4"sv  != scheme &&
               "tcp6"sv  != scheme &&
               "tcp"sv   != scheme)
            {
                return cmt::readyFuture<None>(exception::buildInstance<api::BadAddress>(address.value));
            }

            _bindAddress = std::move(address);
            return cmt::readyFuture(None{});
        };

        //in start();
        methods()->start() += serviceSol() * [this]
        {
            if(_started)
            {
                return;
            }

            _started = true;

            cmt::spawn() += _tow * [this]()
            {
                try
                {
                    idl::gen::net::Host<> host = _hostManager->createService<idl::gen::net::Host<>>().value();

                    _netStreamServer = host->streamServer().value();

                    _netStreamServer->accepted() += _sow * [this](idl::gen::net::stream::Channel<>&& netStreamChannel)
                    {
                        netStreamChannel->setOption(idl::gen::net::option::NoDelay{true});

                        Channel* impl = new Channel(apit::Address{}, std::move(netStreamChannel));
                        impl->involvedChanged() += impl * [impl](bool v)
                        {
                            if(!v)
                            {
                                delete impl;
                            }
                        };

                        methods()->accepted(impl->opposite());
                    };

                    _netStreamServer->failed() += _sow * [this](ExceptionPtr&& e)
                    {
                        methods()->failed(_bindAddress, _boundAddress, std::move(e));
                    };

                    _netStreamServer->closed() += _sow * [this]()
                    {
                        if(_listenDeclared)
                        {
                            _listenDeclared = false;
                            methods()->stopped(_bindAddress, _boundAddress);
                        }
                    };

                    _netStreamServer->setOption(idl::gen::net::option::ReuseAddr{true}).value();

                    _netStreamServer->listen(address2Endpoint(host, _bindAddress)).value();
                    _boundAddress = endpoint2Address(_netStreamServer->localEndpoint().value());
                    methods()->addressChanged(_boundAddress);

                    _listenDeclared = true;
                    methods()->started(_bindAddress, _boundAddress);
                }
                catch(cmt::task::Stop&)
                {
                    _started = false;
                    _sow.flush();

                    if(_netStreamServer)
                    {
                        _netStreamServer->close();
                        _netStreamServer.reset();
                    }

                    if(_listenDeclared)
                    {
                        _listenDeclared = false;
                        methods()->stopped(_bindAddress, _boundAddress);
                    }

                    return;
                }
                catch(...)
                {
                    methods()->failed(_bindAddress, _boundAddress, std::current_exception());
                    if(_listenDeclared)
                    {
                        _listenDeclared = false;
                        methods()->stopped(_bindAddress, _boundAddress);
                    }
                    return;
                }
            };
        };

        //in stop();
        methods()->stop() += serviceSol() * [this]
        {
            _started = false;

            _sow.flush();
            _tow.flush();

            if(_netStreamServer)
            {
                _netStreamServer->close();
                _netStreamServer.reset();
            }

            if(_listenDeclared)
            {
                _listenDeclared = false;
                methods()->stopped(_bindAddress, _boundAddress);
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::~Acceptor()
    {
        _sow.flush();
        _tow.stop();

        if(_netStreamServer)
        {
            _netStreamServer->close();
            _netStreamServer.reset();
        }

        if(_listenDeclared)
        {
            _listenDeclared = false;
            methods()->stopped(_bindAddress, _boundAddress);
        }
    }
}
