// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../pch.hpp"

namespace dci::module::stiac
{
    class LocalEdge;
}

namespace dci::module::stiac::localEdge
{
    class Duty
        : public link::Base
    {
    public:
        Duty(LocalEdge* le);
        ~Duty() override;

        cmt::Future<None> putInterface(Interface&& interface, ILid targetIlid);
        void optimisticPutInterface(Interface&& interface, ILid targetIlid);

        void linkBeginRemove(link::LocalId localId);
        void linkBeginRemove(link::RemoteId remoteId);

        void linkEndRemove(link::LocalId localId);
        void linkEndRemove(link::RemoteId remoteId);

    private:
        void input(link::Source& source) override;
        void destroy() override;

    private:
        LocalEdge* _le;
    };
}
