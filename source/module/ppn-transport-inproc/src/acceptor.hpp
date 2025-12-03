// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::inproc
{
    class Acceptor
        : public api::Acceptor<>::Opposite
        , public host::module::ServiceBase<Acceptor>
    {
    public:
        Acceptor();
        ~Acceptor();

        static Acceptor* findAcceptor(const String& address);

    private:
        apit::Address   _address;
        bool            _started {false};

    private:
        static std::map<String, Acceptor*> _registry;
    };
}
