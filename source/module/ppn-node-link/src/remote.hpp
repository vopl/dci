// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "remote/state.hpp"

namespace dci::module::ppn::node::link
{
    class Local;

    class Remote;
    using RemotePtr = std::unique_ptr<Remote>;

    class Remote
        : public mm::heap::Allocable<Remote>
        , public api::Remote<>::Opposite
        , public sbs::Owner
        , private remote::State
    {
        Remote(Local* local, remote::State&& state);

    public:
        static RemotePtr create(Local* local, remote::State&& state);
        ~Remote();

        void start();

        const remote::State& state() const;

        void payloadOutIdsChanged();

    private:
        void setupPayload();
        void setupPayloadOut();
        void doClose();

    private:
        Local* _local;
        sbs::Owner _sbsOwner4Payload;
    };
}
