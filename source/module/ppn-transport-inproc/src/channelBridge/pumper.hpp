// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::inproc::channelBridge
{
    class Side;

    class Pumper
    {
    public:
        Pumper();
        ~Pumper();

        void want(Side*);
        void unwant(Side*);

    public:
        void worker();

    private:
        cmt::Notifier       _workerWaker;
        cmt::task::Owner    _workerOwner;

        std::set<Side*>     _wants;
    };

    using PumperPtr = Pumper *;
    extern PumperPtr g_pumperPtr;
}
