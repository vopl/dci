// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../local.hpp"

namespace dci::module::ppn::discovery::local
{
    class ProcessScope
        : public Local<ProcessScope, api::ProcessScope<>::Opposite>
    {
    public:
        ProcessScope();
        ~ProcessScope();

        void started();
        void stopped();
        void declared(const Entry& entry);

    private:
        using Randezvous = std::set<ProcessScope*>;
        static Randezvous _randezvous;
    };
}
