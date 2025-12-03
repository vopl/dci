// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/logger.hpp>
#include <dci/utils/dbg.hpp>
#include <dci/utils/win32/error.hpp>
#include <windows.h>

namespace dci::cmt::impl::ctx
{
    template <class Derived>
    class Engine
    {
    public:
        static constexpr bool _needStack = true;

    protected:
        void constructRoot();
        void destructRoot();

        void constructFiber(bool growsDown, char* sptr, std::size_t ssize);
        void destructFiber();

        template <class D2>
        void switchTo(Engine<D2>* to);

    private:
        static void s_call(LPVOID self);

    private:
        template<class D> friend class Engine;
        LPVOID _context{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::constructRoot()
    {
        dbgAssert(!_context);
        _context = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
        if(!_context)
        {
            LOGE("ConvertThreadToFiberEx failed: " << utils::win32::error::last());
            std::terminate();
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::destructRoot()
    {
        dbgAssert(_context);
        _context = {};
        if(!ConvertFiberToThread())
        {
            LOGE("ConvertFiberToThread failed: " << utils::win32::error::last());
            std::terminate();
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::constructFiber(bool /*growsDown*/, char* /*sptr*/, std::size_t /*ssize*/)
    {
        _context = CreateFiberEx(4096, 1024*1024*128, FIBER_FLAG_FLOAT_SWITCH, &Engine::s_call, this);
        if(!_context)
        {
            LOGE("CreateFiberEx failed: " << utils::win32::error::last());
            std::terminate();
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::destructFiber()
    {
        dbgAssert(_context);
        DeleteFiber(std::exchange(_context, {}));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    template <class D2>
    void Engine<Derived>::switchTo(Engine<D2>* to)
    {
        dbgAssert(static_cast<void*>(this) != static_cast<void*>(to));
        SwitchToFiber(to->_context);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::s_call(LPVOID self)
    {
        Derived* derived = static_cast<Derived*>(static_cast<Engine*>(self));
        derived->contextProc();
    }
}
