// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net
{
    class Host;

    class Routes
        : public sbs::Owner
    {
    public:
        Routes(api::Host<>::Opposite* iface);
        ~Routes();

        void add(const api::route::Entry4& e);
        void del(const api::route::Entry4& e);

        void add(const api::route::Entry6& e);
        void del(const api::route::Entry6& e);

        void flushChanges();

    private:
        api::Host<>::Opposite * _iface = nullptr;

        List<api::route::Entry4> _table4;
        List<api::route::Entry6> _table6;

        List<api::route::Entry4> _added4;
        List<api::route::Entry6> _added6;

        List<api::route::Entry4> _deleted4;
        List<api::route::Entry6> _deleted6;
    };
}
