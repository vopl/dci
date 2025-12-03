// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include <dci/primitives/array.hpp>
#include "container/assembly.hpp"
#include "container/face/generic.hpp"
#include "container/face/iterable.hpp"

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class V, std::size_t N>
    struct Def<Array<V, N>>
    {
        using T = Array<V, N>;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        using Proxy = def::container::Assembly<T, def::container::face::Generic, def::container::face::Iterable>;
        using Api = typename Proxy::Api;


        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        static void registerOperations()
        {
            //qmlRegisterAnonymousSequentialContainer<T>("SomeUri", 0);

//            registerConvertorIfMissing([](const QVariantList& o)
//            {
//                T res{};

//                std::size_t size = std::min(res.size(), static_cast<std::size_t>(o.size()));
//                for(std::size_t idx{0}; idx<size; ++idx)
//                {
//                    res[idx] = qvariant_cast<V>(o[idx]);
//                }

//                return res;
//            });

//            registerConvertorIfMissing([](const T& o)
//            {
//                QVariantList res;
//                res.reserve(o.size());
//                for(const V& v : o)
//                {
//                    res << QVariant::fromValue(v);
//                }

//                return res;
//            });

//            registerConvertorIfMissing([](const QJSValue& o)
//            {
//                return qvariant_cast<T>(o.toVariant());
//            });

//            registerConvertorIfMissing([](const T& o)
//            {
//                return QSequentialIterable{&o};
//            });

//            registerMutableViewIfMissing([](T& o)
//            {
//                return QSequentialIterable{&o};
//            });
        }
    };
}
