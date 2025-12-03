// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "cookies/store.hpp"

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Cookies
        : public api::http::client::Cookies<>::Opposite
        , public host::module::ServiceBase<Cookies>
    {
    public:
        Cookies();
        ~Cookies();

    private:
        cookies::Store _store;
    };
}
