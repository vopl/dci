/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"

namespace dci::module::www::agent
{
    template <class T>
    class OwningDList
    {
    public:
        OwningDList() {}
        OwningDList(OwningDList&& from);
        ~OwningDList();

        OwningDList& operator=(OwningDList&& from);

        std::size_t count() const;
        bool empty() const;

        T* first() const;
        T* last() const;

        bool contains(T*);

        void push(T*);
        void release(T*);
        void erase(T*);
        void each(auto&& f);
        void release(auto&& f);
        void erase(auto&& f);
        void clear();

    private:
        dci::utils::IntrusiveDlist<T>   _list;
        std::size_t                     _count{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    OwningDList<T>::OwningDList(OwningDList&& from)
        : _list{std::move(from._list)}
        , _count{std::exchange(from._count, {})}
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    OwningDList<T>& OwningDList<T>::operator=(OwningDList&& from)
    {
        _list = std::move(from._list);
        _count = std::exchange(from._count, {});
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    OwningDList<T>::~OwningDList()
    {
        clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    std::size_t OwningDList<T>::count() const
    {
        return _count;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool OwningDList<T>::empty() const
    {
        return !_count;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T* OwningDList<T>::first() const
    {
        return _list.first();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T* OwningDList<T>::last() const
    {
        return _list.last();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool OwningDList<T>::contains(T* e)
    {
        return _list.contains(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::push(T* e)
    {
        _list.push(e);
        ++_count;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::release(T* e)
    {
        dbgAssert(_list.contains(e));
        dbgAssert(0 < _count);

        _list.remove(e);
        --_count;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::erase(T* e)
    {
        release(e);
        delete e;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::each(auto&& f)
    {
        _list.each([this, &f](T* e)
        {
            std::forward<decltype(f)>(f)(e);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::release(auto&& f)
    {
        _list.flush([this, &f](T* e)
        {
            std::forward<decltype(f)>(f)(e);
            --_count;
        });
        dbgAssert(!_count);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::erase(auto&& f)
    {
        release([&](T* e)
        {
            std::forward<decltype(f)>(f)(e);
            delete e;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void OwningDList<T>::clear()
    {
        erase([](T*){});
    }
}
