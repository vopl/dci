// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "polling.hpp"
#include "clocking.hpp"
#include "awaking.hpp"
#include <system_error>
#include <dci/sbs/wire.hpp>

namespace dci::poll::impl
{
    class Service
    {
    public:
        Service();
        ~Service();

    public:
        std::error_code     initialize();
        std::error_code     run(bool emitStartedStopped);
        sbs::Signal<>       started();
        sbs::Signal<bool>   doSomeWork();
        std::error_code     stop();
        sbs::Signal<>       stopped();
        std::error_code     deinitialize();

    private:
        friend class Timer;
        Clocking& clocking();

    private:
        friend class Descriptor;
        Polling& polling();

    private:
        friend class Awaker;
        Awaking& awaking();

    private:
        Clocking    _clocking;
        Polling     _polling;
        Awaking     _awaking;

        sbs::Wire<>     _started;
        sbs::Wire<bool> _doSomeWork;
        sbs::Wire<>     _stopped;
        bool            _stop{true};
    };

    extern Service& service;
}
