// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::service::dht
{
    class Storage
    {
    public:
        void put(const api::Key& key, const api::Value& value);
        void get(const api::Key& key, uint32 limit, Set<idl::gen::ppn::service::dht::Value> &value);

        void dropOld();

    private:
        struct ValueHolder
        {
            api::Key                                _key;
            api::Value                              _value;
            std::chrono::steady_clock::time_point   _moment;

            ValueHolder(auto&& key, auto&& value, auto&& moment)
                : _key{std::forward<decltype(key)>(key)}
                , _value{std::forward<decltype(value)>(value)}
                , _moment{std::forward<decltype(moment)>(moment)}
            {
            }
        };

        using ByKey = boost::multi_index::member<ValueHolder, api::Key, &ValueHolder::_key>;
        using ByValue = boost::multi_index::member<ValueHolder, api::Value, &ValueHolder::_value>;

        using ByKeyValue = boost::multi_index::composite_key
        <
            ValueHolder,
            ByKey,
            ByValue
        >;

        using ByMoment = boost::multi_index::member<ValueHolder, std::chrono::steady_clock::time_point, &ValueHolder::_moment>;

        using Container = boost::multi_index::multi_index_container
        <
            ValueHolder,
            boost::multi_index::indexed_by
            <
                boost::multi_index::ordered_non_unique <boost::multi_index::tag<ByKey>,         ByKey>,
                boost::multi_index::ordered_non_unique <boost::multi_index::tag<ByKeyValue>,    ByKeyValue>,
                boost::multi_index::ordered_non_unique <boost::multi_index::tag<ByMoment>,      ByMoment>
            >
        >;

        Container _container;
    };
}
