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
