/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <type_traits>
#include <utility>
#include <cstdint>

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class Tag = void>
    struct IntrusiveDlistElement
    {
        IntrusiveDlistElement();
        IntrusiveDlistElement(IntrusiveDlistElement<T, Tag>* prev, IntrusiveDlistElement<T, Tag>* next);

        bool emplaced() const;

        IntrusiveDlistElement* prev() const;
        IntrusiveDlistElement* next() const;

        T* prevT() const;
        T* nextT() const;

    private:
        template <class T2, class Tag2, class RemoveCleaner> friend class IntrusiveDlist;
        void reset();
        void set(IntrusiveDlistElement* prev, IntrusiveDlistElement* next);
        void setPrev(IntrusiveDlistElement* prev);
        void setNext(IntrusiveDlistElement* next);

    private:
        using Int = std::uintptr_t;
        Int _prev{};
        Int _next{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>* intrusiveDlistElementCast(T* e) requires std::is_base_of_v<IntrusiveDlistElement<T, Tag>, T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class Tag>
    T* intrusiveDlistElementCast(IntrusiveDlistElement<T, Tag>* e, T* stubForADL = nullptr) requires std::is_base_of_v<IntrusiveDlistElement<T, Tag>, T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct NullRemoveCleaner
    {
        template <class... T>
        void operator()(const T&...) {}
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class Tag = void, class RemoveCleaner=NullRemoveCleaner>
    class IntrusiveDlist
        : private RemoveCleaner
    {
        IntrusiveDlist(const IntrusiveDlist&) = delete;
        void operator=(const IntrusiveDlist&) = delete;

    public:
        IntrusiveDlist();
        IntrusiveDlist(T* element);
        IntrusiveDlist(RemoveCleaner&& removeCleaner);
        IntrusiveDlist(T* element, RemoveCleaner&& removeCleaner);

        template <class Tag2, class RC2>
        IntrusiveDlist(IntrusiveDlist<T, Tag2, RC2>&& from);

        template <class Tag2, class RC2>
        IntrusiveDlist& operator=(IntrusiveDlist<T, Tag2, RC2>&& from);

        ~IntrusiveDlist();

    public:
        bool empty() const;
        T* first() const;
        T* last() const;
        std::pair<T*, T*> range() const;
        bool contains(T* element) const;
        void push(T* element);
        T* shift();
        void remove(T* element);

        void clear();

        template <class F>
        void each(F&& f);

        template <class F>
        void each(F&& f) const;

        template <class F>
        void flush(F&& f);

    private:
        IntrusiveDlistElement<T, Tag>* _first{};
        IntrusiveDlistElement<T, Tag>* _last{};
    };
}

#include "intrusiveDlist.ipp"
