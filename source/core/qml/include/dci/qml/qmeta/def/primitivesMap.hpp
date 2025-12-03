// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include <dci/primitives/map.hpp>
#include "container/assembly.hpp"
#include "container/face/generic.hpp"

//#include <QAssociativeIterable>
//#include <QJSValue>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class K, class V>
    struct Def<Map<K, V>>
    {
        using T = Map<K, V>;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        using Proxy = def::container::Assembly<T, def::container::face::Generic>;
        using Api = typename Proxy::Api;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        static void registerOperations()
        {
//            registerConvertorIfMissing([](const QJSValue& o)
//            {
//                return qvariant_cast<T>(o.toVariant());
//            });

//            registerConvertorIfMissing([](const T& o)
//            {
//                return QIterable<QMetaAssociation>{QMetaAssociation::fromContainer<T>(), &o};
//            });

//            registerMutableViewIfMissing([](T& o)
//            {
//                return QIterable<QMetaAssociation>{QMetaAssociation::fromContainer<T>(), &o};
//            });
        }
    };
}
