// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
