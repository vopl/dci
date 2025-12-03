// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"
#include "../optionsStore.hpp"

namespace dci::module::net
{
    class Host;

    namespace stream
    {
        class Client
            : public api::stream::Client<>::Opposite
            , public sbs::Owner
            , public mm::heap::Allocable<Client>
            , private OptionsStore
        {
        public:
            Client(Host* host);
            ~Client();

        private:

        private:
            Host *          _host;
            api::Endpoint   _bindEndpoint;
            bool            _binded = false;
        };
    }
}
