// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include <dci/primitives/set.hpp>
#include "container/assembly.hpp"
#include "container/face/generic.hpp"

//#include <QSequentialIterable>
//#include <QJSValue>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class V>
    struct Def<Set<V>>
    {
        using T = Set<V>;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        using Proxy = def::container::Assembly<T, def::container::face::Generic>;
        using Api = typename Proxy::Api;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        static void registerOperations()
        {
//            registerConvertorIfMissing([](const QVariantList& o)
//            {
//                T res;
//                //res.reserve(o.size());
//                for(const QVariant& v : o)
//                {
//                    res.emplace(qvariant_cast<V>(v));
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
