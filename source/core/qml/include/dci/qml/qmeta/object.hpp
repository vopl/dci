/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "def.hpp"
#include "superdata.hpp"
#include "stringdata.hpp"
#include "data.hpp"
#include "call.hpp"
#include <QMetaObject>

namespace dci::qml::qmeta
{
    namespace object::details
    {
        template <class T, class=void>
        struct RegisterOperations{static bool exec(){ return 0; }};

        template <class T>
        struct RegisterOperations<T, typename std::void_t<decltype(Def<T>::registerOperations())>>
        {
            static bool exec()
            {
                Def<T>::registerOperations();
                return 1;
            }
        };
    }

    template <class T>
    requires (Def<T>::_declared)
    struct Object
    {
    private:
        static bool _registerOperationsUtilizer;

    public:
        static const QMetaObject* qt()
        {
            (void)_registerOperationsUtilizer;

            static const QMetaObject result =[]
            {
                return QMetaObject
                {
                    Superdata<T>::qt(),     //SuperData superdata;
                    Stringdata<T>::qt(),    //const uint *stringdata;
                    Data<T>::qt(),          //const uint *data;
                    Call<T>::qt(),          //StaticMetacallFunction static_metacall;
                    {},                     //const SuperData *relatedMetaObjects;
                    Data<T>::qtMetaTypes(), //const QtPrivate::QMetaTypeInterface *const *metaTypes;
                    {},                     //void *extradata; //reserved for future use
                };
            }();

            return &result;
        }
    };

    template <class T>
    requires (Def<T>::_declared)
    bool Object<T>::_registerOperationsUtilizer = object::details::RegisterOperations<T>::exec();
}
