// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include <dci/host/daemonBase.hpp>
#include "node.hpp"
#include <memory>

namespace dci::module::ppn::node
{
    class Daemon
        : public host::DaemonBase<Daemon>
    {
    public:
        Daemon();
        ~Daemon();

        void startImpl(idl::gen::Config&& config);
        void stopImpl();
        idl::Interface serviceImpl();

    private:
        std::unique_ptr<Node>       _node;
    };
}
