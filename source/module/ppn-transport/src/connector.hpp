// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "endpoints.hpp"

namespace dci::module::ppn::transport
{
    class Connector
        : public api::Connector<>::Opposite
        , public host::module::ServiceBase<Connector>
    {
    public:
        Connector();
        ~Connector();

        api::connector::Downstream<> getBestDownstreamFor(const api::Address& address, const Set<api::connector::Downstream<>>& blacklist);

    private:
        cmt::task::Owner _tol;
        Endpoints<api::connector::Downstream<>> _endpoints;
    };
}
