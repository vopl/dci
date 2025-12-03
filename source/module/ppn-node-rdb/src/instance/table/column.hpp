// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
