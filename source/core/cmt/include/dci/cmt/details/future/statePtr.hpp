/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/details/future/state.hpp>

namespace dci::cmt::details::future
{
    template <class T, bool forFuture>
    class StatePtr
    {
    public:
        using State = future::State<T>;

    public:
        StatePtr();
        ~StatePtr();

        StatePtr(State* raw);
        StatePtr(const StatePtr& from);
        StatePtr(StatePtr&& from);

        StatePtr& operator=(const StatePtr& from);
        StatePtr& operator=(StatePtr&& from);

        const State* raw() const;
        const State* operator->() const;

        State* raw();
        State* operator->();

        operator bool() const;

        bool charged() const;
        void uncharge();

    private:
        void addRef();
        void release();

    private:
        State* _raw = nullptr;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>::StatePtr()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>::~StatePtr()
    {
        release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>::StatePtr(State* raw)
        : _raw(raw)
    {
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>::StatePtr(const StatePtr& from)
        : _raw(from._raw)
    {
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>::StatePtr(StatePtr&& from)
        : _raw(from._raw)
    {
        from._raw = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>& StatePtr<T, forFuture>::operator=(const StatePtr& from)
    {
        if(_raw != from._raw)
        {
            release();
            _raw = from._raw;
            addRef();
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>& StatePtr<T, forFuture>::operator=(StatePtr&& from)
    {
        if(_raw != from._raw)
        {
            release();
            _raw = from._raw;
            from._raw = nullptr;
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    const typename StatePtr<T, forFuture>::State* StatePtr<T, forFuture>::raw() const
    {
        return _raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    const typename StatePtr<T, forFuture>::State* StatePtr<T, forFuture>::operator->() const
    {
        dbgAssert(_raw);
        return _raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    typename StatePtr<T, forFuture>::State* StatePtr<T, forFuture>::raw()
    {
        return _raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    typename StatePtr<T, forFuture>::State* StatePtr<T, forFuture>::operator->()
    {
        dbgAssert(_raw);
        return _raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    StatePtr<T, forFuture>::operator bool() const
    {
        return !!_raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    bool StatePtr<T, forFuture>::charged() const
    {
        return !!_raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    void StatePtr<T, forFuture>::uncharge()
    {
        release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    void StatePtr<T, forFuture>::addRef()
    {
        if(_raw)
        {
            if(forFuture)
            {
                _raw->incFutureCounter();
            }
            else
            {
                _raw->incPromiseCounter();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, bool forFuture>
    void StatePtr<T, forFuture>::release()
    {
        if(_raw)
        {
            if(forFuture)
            {
                _raw->decFutureCounter();
            }
            else
            {
                _raw->decPromiseCounter();
            }
            _raw = nullptr;
        }
    }
}
