/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/idl/introspection.hpp>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept Struct = idl::introspection::isStruct<T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Struct T>
    void save(auto& ar, const T& v)
    {
        idl::introspection::applyBases([&](const auto&... v)
        {
            (void)(ar << ... << v);
        }, v);

        idl::introspection::applyFields([&](const auto&... v)
        {
            (void)(ar << ... << v);
        }, v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Struct T>
    void save(auto& ar, T&& v)
    {
        idl::introspection::applyBases([&](auto&&... v)
        {
            (void)(ar << ... << std::move(v));
        }, std::move(v));

        idl::introspection::applyFields([&](auto&&... v)
        {
            (void)(ar << ... << std::move(v));
        }, std::move(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Struct T>
    void load(auto& ar, T& v)
    {
        idl::introspection::applyBases([&](auto&... v)
        {
            (void)(ar >> ... >> v);
        }, v);

        idl::introspection::applyFields([&](auto&... v)
        {
            (void)(ar >> ... >> v);
        }, v);
    }
}
