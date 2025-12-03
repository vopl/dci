// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
    bool IntrusiveDlistElement<T, Tag>::emplaced() const
    {
        return !!_prev;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>* IntrusiveDlistElement<T, Tag>::prev() const
    {
        return reinterpret_cast<IntrusiveDlistElement<T, Tag>*>(_prev & ~Int{1});
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    IntrusiveDlistElement<T, Tag>* IntrusiveDlistElement<T, Tag>::next() const
    {
        return reinterpret_cast<IntrusiveDlistElement<T, Tag>*>(_next & ~Int{1});
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
        _prev = _next = Int{};
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
        _prev = reinterpret_cast<Int>(prev) | 1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag>
    void IntrusiveDlistElement<T, Tag>::setNext(IntrusiveDlistElement<T, Tag>* next)
    {
        _next = reinterpret_cast<Int>(next) | 1;
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
        : _first{intrusiveDlistElementCast<T, Tag>(element)}
        , _last{intrusiveDlistElementCast<T, Tag>(element)}
    {
        auto idee = intrusiveDlistElementCast<T, Tag>(element);
        idee->set(nullptr, nullptr);
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
        : _first{intrusiveDlistElementCast<T, Tag>(element)}
        , _last{intrusiveDlistElementCast<T, Tag>(element)}
        , RemoveCleaner{std::forward<RemoveCleaner>(removeCleaner)}
    {
        auto idee = intrusiveDlistElementCast<T, Tag>(element);
        idee->set(nullptr, nullptr);
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

        _first = std::exchange(from._first, {});
        _last = std::exchange(from._last, {});

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    IntrusiveDlist<T, Tag, RemoveCleaner>::~IntrusiveDlist()
    {
        dbgAssert(!_first && !_last);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    bool IntrusiveDlist<T, Tag, RemoveCleaner>::empty() const
    {
        dbgAssert(!_first == !_last);
        return !_first;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    T* IntrusiveDlist<T, Tag, RemoveCleaner>::first() const
    {
        return intrusiveDlistElementCast<T, Tag>(_first);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    T* IntrusiveDlist<T, Tag, RemoveCleaner>::last() const
    {
        return intrusiveDlistElementCast<T, Tag>(_last);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    std::pair<T*, T*> IntrusiveDlist<T, Tag, RemoveCleaner>::range() const
    {
        return std::pair{intrusiveDlistElementCast<T, Tag>(_first), intrusiveDlistElementCast<T, Tag>(_last)};
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    bool IntrusiveDlist<T, Tag, RemoveCleaner>::contains(T* element) const
    {
        IntrusiveDlistElement<T, Tag>* idee = intrusiveDlistElementCast<T, Tag>(element);
        while(idee->prev()) idee = idee->prev();
        return idee == _first;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::push(T* element)
    {
        auto idee = intrusiveDlistElementCast<T, Tag>(element);
        dbgAssert(!idee->emplaced());

        if(_last)
        {
            dbgAssert(_first);

            dbgAssert(!_last->next());
            _last->setNext(idee);
            idee->set(_last, nullptr);
            _last = idee;
        }
        else
        {
            dbgAssert(!_first);

            _first = _last = idee;
            idee->set(nullptr, nullptr);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    T* IntrusiveDlist<T, Tag, RemoveCleaner>::shift()
    {
        if(!_first)
            return nullptr;

        IntrusiveDlistElement<T, Tag>* idee = _first;

        if(_first->next())
        {
            _first = _first->next();
            _first->setPrev(nullptr);
        }
        else
        {
            dbgAssert(_first == _last);
            dbgAssert(!_first->prev());
            _first = _last = nullptr;
        }

        idee->reset();

        // RemoveCleaner::operator ()(result);
        return intrusiveDlistElementCast<T, Tag>(idee);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T, class Tag, class RemoveCleaner>
    void IntrusiveDlist<T, Tag, RemoveCleaner>::remove(T* element)
    {
        dbgAssert(contains(element));

        auto idee = intrusiveDlistElementCast<T, Tag>(element);

        if(idee == _first)
        {
            if(idee == _last)
                _first = _last = nullptr;
            else
            {
                idee->next()->setPrev(nullptr);
                _first = idee->next();
            }
        }
        else if(idee == _last)
        {
            idee->prev()->setNext(nullptr);
            _last = idee->prev();
        }
        else
        {
            idee->prev()->setNext(idee->next());
            idee->next()->setPrev(idee->prev());
        }

        idee->reset();

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
        IntrusiveDlistElement<T, Tag>* idee = _first;
        while(idee)
        {
            IntrusiveDlistElement<T, Tag>* next = idee->next();

            T* element = intrusiveDlistElementCast<T, Tag>(idee);
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
    void IntrusiveDlist<T, Tag, RemoveCleaner>::each(F&& f) const
    {
        IntrusiveDlistElement<T, Tag>* idee = _first;
        while(idee)
        {
            IntrusiveDlistElement<T, Tag>* next = idee->next();

            const T* element = intrusiveDlistElementCast<T, Tag>(idee);
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
        IntrusiveDlistElement<T, Tag>* idee = _first;
        _first = _last = nullptr;
        while(idee)
        {
            IntrusiveDlistElement<T, Tag>* next = idee->next();
            idee->reset();
            T* element = intrusiveDlistElementCast<T, Tag>(idee);
            f(element);
            RemoveCleaner::operator ()(element);
            idee = next;
        }
    }
}
