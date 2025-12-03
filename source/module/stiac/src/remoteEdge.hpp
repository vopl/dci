// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "stages/base.hpp"

namespace dci::module::stiac
{
    class RemoteEdge
        : public stages::Base
        , private sbs::Owner
    {
    public:
        RemoteEdge(Protocol* protocol, const api::RemoteEdge<>::Opposite& interface);
        ~RemoteEdge() override;

    private:
        void input(Bytes&& msg) override;

    private:
        api::RemoteEdge<>::Opposite _interface;
    };

    using RemoteEdgePtr = std::unique_ptr<RemoteEdge>;
}
