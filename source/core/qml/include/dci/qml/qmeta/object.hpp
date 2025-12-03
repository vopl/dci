// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
