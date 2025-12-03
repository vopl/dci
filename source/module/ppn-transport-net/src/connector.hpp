// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::net
{
    class Connector
        : public apit::net::Connector<>::Opposite
        , public host::module::ServiceBase<Connector>
    {
    public:
        Connector(host::Manager* hostManager);
        ~Connector();

    private:
        host::Manager *                                 _hostManager;
        cmt::Future<idl::gen::net::Host<>>              _netHost;
        cmt::Future<idl::gen::net::stream::Client<>>    _netStreamClient;

        apit::Address                                   _address;

        cmt::task::Owner                                _tol;
    };
}
