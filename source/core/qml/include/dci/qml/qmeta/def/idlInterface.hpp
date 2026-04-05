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
#include "../api/meth.hpp"
#include "../api/prop.hpp"
#include <dci/idl/introspection.hpp>
#include <dci/idl/interface.hpp>
#include <dci/cmt/future.hpp>
#include <QMetaType>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T_>
    requires (idl::introspection::isInterface<T_>)
    struct Def<T_>
    {
        using T = T_;
        static constexpr bool _declared = true;
        static constexpr Name _name {typeName<T>};

        template <auto MI, bool=methodIn<T, MI>, class=MakeSeq<methodParamsCount<T, MI>>>
        struct GetMethodApiImpl
        {
            using Result = api::PropR
            <
                //Name<methodName<T, MI>.size()-1>{methodName<T, MI>},//gcc ICE https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99119
                methodName<T, MI>,
                methodActivator<T, MI>
            >;
        };

        template <auto MI, auto... PI>
        struct GetMethodApiImpl<MI, true, VList<PI...>>
        {
            using Result = api::Meth
            <
                methodName<T, MI>,
                [](T& i, const methodParamType<T, MI, PI>&... params) {return methodActivator<T, MI>(i, params...);},
                methodParamName<T, MI, PI>...
            >;
        };

        template <class MI>
        using GetMethodApi = typename GetMethodApiImpl<MI::_v>::Result;
        using Api = typename MakeSeq<methodsCount<T>>::template Map<GetMethodApi>;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        static void registerOperations()
        {
            auto iterate = []<class C, auto... BI>(auto self, TList<C>, VList<BI...>)
            {
                ((QMetaType::hasRegisteredConverterFunction<T, baseType<C, BI>>() ? true : QMetaType::registerConverter<T, baseType<C, BI>>()), ...);
                ((QMetaType::hasRegisteredConverterFunction<baseType<C, BI>, T>() ? true : QMetaType::registerConverter<baseType<C, BI>, T>()), ...);

                (void)self;
                (self(
                    self,
                    List<baseType<C, BI>>{},
                    MakeSeq<basesCount<baseType<C, BI>>>{}
                ), ...);
            };

            iterate(iterate, TList<T>{}, MakeSeq<basesCount<T>>{});

            if(!QMetaType::hasRegisteredConverterFunction<T, idl::Interface>())
            {
                QMetaType::registerConverter<T, idl::Interface>();
            }

            if(!QMetaType::hasRegisteredConverterFunction<idl::Interface, T>())
            {
                QMetaType::registerConverter<idl::Interface, T>();
            }
        }
    };
}
