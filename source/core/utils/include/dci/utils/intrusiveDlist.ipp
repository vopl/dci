/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "intrusiveDlist.hpp"
#include <concepts>
#include <dci/utils/dbg.hpp>

namespace dci::utils
{
    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>::IntrusiveDlistElement()
        : _prev{}
        , _next{}
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>::IntrusiveDlistElement(IntrusiveDlistElement<T, Tag>* prev, IntrusiveDlistElement<T, Tag>* next)
    {
        set(prev, next);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>::~IntrusiveDlistElement()
    {
        retire();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    bool IntrusiveDlistElement<T, Tag>::emplaced() const
    {
        return !!prev();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    void IntrusiveDlistElement<T, Tag>::retire()
    {
        if(emplaced())
        {
            dbgAssert(this != next());
            dbgAssert(this != prev());

            next()->setPrev(prev());
            prev()->setNext(next());
            set({}, {});
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>* IntrusiveDlistElement<T, Tag>::prev() const
    {
        return _prev;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>* IntrusiveDlistElement<T, Tag>::next() const
    {
        return _next;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    T* IntrusiveDlistElement<T, Tag>::selfT()
    {
        return intrusiveDlistElementCast<T, Tag>(this);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    const T* IntrusiveDlistElement<T, Tag>::selfT() const
    {
        return intrusiveDlistElementCast<T, Tag>(this);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    T* IntrusiveDlistElement<T, Tag>::prevT() const
    {
        return intrusiveDlistElementCast<T, Tag>(prev());
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    T* IntrusiveDlistElement<T, Tag>::nextT() const
    {
        return intrusiveDlistElementCast<T, Tag>(next());
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    void IntrusiveDlistElement<T, Tag>::reset()
    {
        _prev = _next = {};
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    void IntrusiveDlistElement<T, Tag>::set(IntrusiveDlistElement<T, Tag>* prev, IntrusiveDlistElement<T, Tag>* next)
    {
        setPrev(prev);
        setNext(next);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    void IntrusiveDlistElement<T, Tag>::setPrev(IntrusiveDlistElement<T, Tag>* prev)
    {
        _prev = prev;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    void IntrusiveDlistElement<T, Tag>::setNext(IntrusiveDlistElement<T, Tag>* next)
    {
        _next = next;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>* intrusiveDlistElementCast(T* e) requires std::is_base_of_v<IntrusiveDlistElement<T, Tag>, T>
    {
        return static_cast<IntrusiveDlistElement<T, Tag> *>(e);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    T* intrusiveDlistElementCast(IntrusiveDlistElement<T, Tag>* e, T* stubForADL) requires std::is_base_of_v<IntrusiveDlistElement<T, Tag>, T>
    {
        (void)stubForADL;
        return static_cast<T *>(e);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist(T* element)
        : IntrusiveDlist{intrusiveDlistElementCast<T, Tag>(element)}
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist(IntrusiveDlistElement<T, Tag>* idee)
        : _center{idee, idee}
    {
        idee->set(&_center, &_center);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist(RemoveCleaner&& removeCleaner)
        : RemoveCleaner{std::forward<RemoveCleaner>(removeCleaner)}
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist(T* element, RemoveCleaner&& removeCleaner)
        : IntrusiveDlist{intrusiveDlistElementCast<T, Tag>(element), std::forward<RemoveCleaner>(removeCleaner)}
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist(IntrusiveDlistElement<T, Tag>* idee, RemoveCleaner&& removeCleaner)
        : _center{idee, idee}
        , RemoveCleaner{std::forward<RemoveCleaner>(removeCleaner)}
    {
        idee->set(&_center, &_center);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    template <class Tag2, class RC2>
    IntrusiveDlist<T, Tag, RemoveCleaner>::IntrusiveDlist(IntrusiveDlist<T, Tag2, RC2>&& from)
    {
        operator=(std::move(from));
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    template <class Tag2, class RC2>
    IntrusiveDlist<T, Tag, RemoveCleaner>& IntrusiveDlist<T, Tag, RemoveCleaner>::operator=(IntrusiveDlist<T, Tag2, RC2>&& from)
    {
        clear();

        if(from.empty())
            return *this;

        _center.set(from._center.prev(), from._center.next());
        _center.prev()->setNext(&_center);
        _center.next()->setPrev(&_center);

        from._center.set(&from._center, &from._center);

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::~IntrusiveDlist()
    {
        dbgAssert(empty());
        clear();
        _center.set({}, {});
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    bool IntrusiveDlist<T, Tag, RemoveCleaner>::empty() const
    {
        return _center.next() == &_center;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    T* IntrusiveDlist<T, Tag, RemoveCleaner>::first() const
    {
        if(empty())
            return {};
        return _center.nextT();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    T* IntrusiveDlist<T, Tag, RemoveCleaner>::last() const
    {
        if(empty())
            return {};
        return _center.prevT();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    std::pair<T*, T*> IntrusiveDlist<T, Tag, RemoveCleaner>::range() const
    {
        return {first(), last()};
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    bool IntrusiveDlist<T, Tag, RemoveCleaner>::contains(T* element) const
    {
        IntrusiveDlistElement<T, Tag>* idee = intrusiveDlistElementCast<T, Tag>(element);
        if(!idee->emplaced())
            return false;

        IntrusiveDlistElement<T, Tag>* idee0 = _center.prev();

        while(idee0 != &_center)
        {
            if(idee == idee0 || idee == &_center)
                return true;

            idee0 = idee0->prev();
            if(idee == idee0)
                return true;

            idee = idee->next();
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::pushBack(T* element)
    {
        auto idee = intrusiveDlistElementCast<T, Tag>(element);
        dbgAssert(!idee->emplaced());

        idee->set(_center.prev(), &_center);
        _center.prev()->setNext(idee);
        _center.setPrev(idee);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    T* IntrusiveDlist<T, Tag, RemoveCleaner>::popFront()
    {
        if(empty())
            return {};

        IntrusiveDlistElement<T, Tag>* idee = _center.next();
        idee->retire();
        // RemoveCleaner::operator ()(idee->selfT());
        return idee->selfT();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::remove(T* element)
    {
        dbgAssert(contains(element));

        auto idee = intrusiveDlistElementCast<T, Tag>(element);
        idee->retire();

        RemoveCleaner::operator ()(element);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::clear()
    {
        flush([](T*){});
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    template <class F>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::each(F&& f)
    {
        IntrusiveDlistElement<T, Tag>* idee = _center.next();
        while(idee != &_center)
        {
            IntrusiveDlistElement<T, Tag>* next = idee->next();

            T* element = idee->selfT();
            if constexpr(requires { {f(element)} -> std::convertible_to<bool>; })
            {
                if(!f(element))
                    break;
            }
            else
                f(element);

            idee = next;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    template <class F>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::flush(F&& f)
    {
        IntrusiveDlistElement<T, Tag>* idee = _center.next();
        _center.set(&_center, &_center);
        while(idee != &_center)
        {
            IntrusiveDlistElement<T, Tag>* next = idee->next();
            idee->reset();
            T* element = idee->selfT();
            f(element);
            RemoveCleaner::operator ()(element);
            idee = next;
        }
    }
}
