// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net
{
    class Links;
    class Routes;

    namespace enumerator
    {
        class Rtnetlink
        {
        public:
            Rtnetlink(Links* links, Routes* routes);
            ~Rtnetlink();

        private:
            void doNextRequest();
            bool request(uint32 type);
            void onSock(int fd, poll::descriptor::ReadyStateFlags readyState);


        private:
            Links *                             _links {};
            Routes *                            _routes {};
            std::unique_ptr<poll::Descriptor>   _sock {};
            sockaddr_nl                         _address {};
            iovec                               _msgiov {};
            msghdr                              _msg {};

        private:
            enum StateFlags
            {
                sf_null             = 0x00,

                sf_requestLink      = 0x01,
                sf_requestAddr      = 0x02,
                sf_requestRoute     = 0x04,

                sf_responseLink     = 0x10,
                sf_responseAddr     = 0x20,
                sf_responseRoute    = 0x40,
            };

            int _state {};
        };
    }
}
