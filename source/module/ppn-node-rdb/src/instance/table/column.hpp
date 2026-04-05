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

namespace dci::module::ppn::node::rdb::instance::table
{
    template <class T>
    class Column final
    {
    public:
        Column(const pql::Column& spec);

    public:
        const pql::Column& spec() const;

        std::size_t recordsAmount() const;

        void insert();
        void remove(std::size_t recordIdx);
        void set(std::size_t recordIdx, T&& v);
        void set(std::size_t recordIdx, const T& v);
        void reset(std::size_t recordIdx);
        T& access(std::size_t recordIdx);
        const T& get(std::size_t recordIdx) const;

    protected:
        const pql::Column   _spec;
        std::deque<T>       _values;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Column<T>::Column(const pql::Column& spec)
        : _spec(spec)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    const pql::Column& Column<T>::spec() const
    {
        return _spec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    std::size_t Column<T>::recordsAmount() const
    {
        return _values.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Column<T>::insert()
    {
        _values.emplace_back(T{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Column<T>::remove(std::size_t recordIdx)
    {
        dbgAssert(recordIdx < _values.size());

        _values[recordIdx] = std::move(_values.back());
        _values.pop_back();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Column<T>::set(std::size_t recordIdx, T&& v)
    {
        dbgAssert(recordIdx < _values.size());
        _values[recordIdx] = std::move(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Column<T>::set(std::size_t recordIdx, const T& v)
    {
        dbgAssert(recordIdx < _values.size());
        _values[recordIdx] = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Column<T>::reset(std::size_t recordIdx)
    {
        dbgAssert(recordIdx < _values.size());
        _values[recordIdx] = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T& Column<T>::access(std::size_t recordIdx)
    {
        dbgAssert(recordIdx < _values.size());
        return _values[recordIdx];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    const T& Column<T>::get(std::size_t recordIdx) const
    {
        dbgAssert(recordIdx < _values.size());
        return _values[recordIdx];
    }
}
