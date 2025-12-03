// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::net
{
    class Acceptor
        : public apit::net::Acceptor<>::Opposite
        , public host::module::ServiceBase<Acceptor>
    {
    public:
        Acceptor(host::Manager* hostManager);
        ~Acceptor();

    private:
        String scopeValue() const;

    private:
        host::Manager *                 _hostManager;
        apit::Address                   _bindAddress;
        apit::Address                   _boundAddress;
        idl::gen::net::stream::Server<> _netStreamServer;
        sbs::Owner                      _sow;
        cmt::task::Owner                _tow;
        bool                            _started = false;
        bool                            _listenDeclared = false;
    };
}
