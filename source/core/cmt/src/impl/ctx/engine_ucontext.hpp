/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/logger.hpp>
#include <dci/utils/dbg.hpp>
#include <ucontext.h>
#include <cstdint>
#include <exception>

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
        static void s_call(int addrLo, int addrHi);

    private:
        template<class D> friend class Engine;
        ucontext_t _ctx;
    };









    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::constructRoot()
    {
        if(getcontext(&_ctx))
        {
            LOGE(__FUNCTION__<<", getcontext failed");
            std::terminate();
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::destructRoot()
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::constructFiber(bool growsDown, char* sptr, std::size_t ssize)
    {
        (void)growsDown;

        if(getcontext(&_ctx))
        {
            LOGE(__FUNCTION__<<", getcontext failed");
            std::terminate();
            return;
        }

        _ctx.uc_link = NULL;
        _ctx.uc_stack.ss_sp = sptr;
        _ctx.uc_stack.ss_size = ssize;

#if INTPTR_MAX == INT32_MAX
        makecontext(this, (void(*)())&Engine::s_call, 2, this, 0);
#elif INTPTR_MAX == INT64_MAX
        std::uintptr_t addr = reinterpret_cast<uintptr_t>(this);
        int addrLo = int(unsigned((addr      ) & 0x00000000ffffffff));
        int addrHi = int(unsigned((addr >> 32) & 0x00000000ffffffff));
        makecontext(&_ctx, reinterpret_cast<void(*)()>(&Engine::s_call), 2, addrLo, addrHi);
#else
#error "Environment not 32 or 64-bit."
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::destructFiber()
    {
        //empry is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    template <class D2>
    void Engine<Derived>::switchTo(Engine<D2>* to)
    {
        dbgAssert(static_cast<void*>(this) != static_cast<void*>(to));

        swapcontext(&_ctx, &to->_ctx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::s_call(int addrLo, int addrHi)
    {
#if INTPTR_MAX == INT32_MAX
        dbgAssert(0 == addrHi);
        Engine* self = reinterpret_cast<Engine*>(addrLo);
#elif INTPTR_MAX == INT64_MAX
        std::uint64_t addr = std::uintptr_t(unsigned(addrLo)) | (std::uintptr_t(unsigned(addrHi))<<32);
        Engine* self = reinterpret_cast<Engine*>(addr);
#else
#error "Environment not 32 or 64-bit."
#endif

        Derived* derived = static_cast<Derived*>(self);
        derived->contextProc();
    }

}
