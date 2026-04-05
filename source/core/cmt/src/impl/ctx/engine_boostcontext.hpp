/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <cstdint>
#include <boost/context/detail/fcontext.hpp>
#include <dci/utils/dbg.hpp>

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

        struct Trans
        {
            void* _callee;
            boost::context::detail::fcontext_t* _callerCtxPointer = nullptr;
        };

        template <class D2>
        void switchTo(Engine<D2>* to);

    private:
        static void s_call(boost::context::detail::transfer_t transfer);

    private:
        template<class D> friend class Engine;
        boost::context::detail::fcontext_t _ctx{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::constructRoot()
    {
        dbgAssert(!_ctx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::destructRoot()
    {
        _ctx = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::constructFiber(bool growsDown, char* sptr, std::size_t ssize)
    {
        dbgAssert(!_ctx);

        if(growsDown)
        {
            _ctx = boost::context::detail::make_fcontext(
                      sptr + ssize,
                      ssize,
                      &s_call);
        }
        else
        {
            _ctx = boost::context::detail::make_fcontext(
                      sptr,
                      ssize,
                      &s_call);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::destructFiber()
    {
        dbgAssert(_ctx);
        _ctx = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    template <class D2>
    void Engine<Derived>::switchTo(Engine<D2>* to)
    {
        dbgAssert(static_cast<void*>(this) != static_cast<void*>(to));

        Trans trans {to, &_ctx};
        boost::context::detail::transfer_t transfer = boost::context::detail::jump_fcontext(to->_ctx, &trans);

        {
            Trans* trans = static_cast<Trans*>(transfer.data);
            dbgAssert(trans->_callerCtxPointer);
            *trans->_callerCtxPointer = transfer.fctx;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived>
    void Engine<Derived>::s_call(boost::context::detail::transfer_t transfer)
    {
        Trans* trans = static_cast<Trans*>(transfer.data);

        dbgAssert(trans->_callerCtxPointer);
        *trans->_callerCtxPointer = transfer.fctx;

        Engine* engine = static_cast<Engine*>(trans->_callee);
        Derived* derived = static_cast<Derived*>(engine);
        derived->contextProc();
    }
}

