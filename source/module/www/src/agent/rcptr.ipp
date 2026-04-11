/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "rcptr.hpp"
#include "refCounted.hpp"

namespace dci::module::www::agent
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>::RCPtr(T* obj)
        : _obj{obj}
    {
        if(_obj)
            static_cast<RefCounted<T>*>(_obj)->incRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>::RCPtr(const RCPtr& other)
        : _obj{other._obj}
    {
        if(_obj)
            static_cast<RefCounted<T>*>(_obj)->incRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>::RCPtr(RCPtr&& other)
        : _obj{std::exchange(other._obj, {})}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>::~RCPtr()
    {
        if(_obj)
            static_cast<RefCounted<T>*>(_obj)->decRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>& RCPtr<T>::operator=(const RCPtr& other)
    {
        T* obj{_obj};

        _obj = other._obj;
        if(_obj)
            static_cast<RefCounted<T>*>(_obj)->incRef();

        if(obj)
            static_cast<RefCounted<T>*>(obj)->decRef();

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>& RCPtr<T>::operator=(RCPtr&& other)
    {
        if(_obj == other._obj)
            other.reset();
        else
        {
            T* obj{_obj};

            _obj = std::exchange(other._obj, {});

            if(obj)
                static_cast<RefCounted<T>*>(obj)->decRef();
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void RCPtr<T>::reset()
    {
        if(_obj)
            static_cast<RefCounted<T>*>(std::exchange(_obj, {}))->decRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    RCPtr<T>::operator bool() const
    {
        return !!_obj;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool RCPtr<T>::operator!() const
    {
        return !_obj;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T* RCPtr<T>::operator->() const
    {
        dbgAssert(_obj);
        return _obj;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T& RCPtr<T>::operator*() const
    {
        dbgAssert(_obj);
        return *_obj;
    }
}
