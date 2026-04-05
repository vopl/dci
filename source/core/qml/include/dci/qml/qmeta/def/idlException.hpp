/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../def.hpp"
#include "../api/prop.hpp"
#include <dci/idl/introspection.hpp>
#include <QMetaType>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T_>
    requires (idl::introspection::isException<T_>)
    struct Def<T_>
    {
        using T = T_;
        static constexpr bool _declared = true;
        static constexpr Name _name {typeName<T>};

        template <auto FI>
        struct GetFieldApiImpl
        {
            using Result = api::PropRW
            <
                fieldName<T, FI>,
                [](const T& s){return fieldValue<T, FI>(s);},
                [](T& s, const fieldType<T, FI>& f){fieldValue<T, FI>(s) = f;}
            >;
        };

        template <class MI>
        using GetFieldApi = typename GetFieldApiImpl<MI::_v>::Result;
        using Api = typename MakeSeq<fieldsCount<T>>::template Map<GetFieldApi>;

        //TODO super
    };
}
