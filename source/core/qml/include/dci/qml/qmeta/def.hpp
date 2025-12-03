// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class>
    struct Def
    {
        static constexpr bool _declared = false;
    };
}


#include "def/cmtFuture.hpp"

#include "def/sbsSignal.hpp"
#include "def/sbsOwner.hpp"

#include "def/idlInterface.hpp"
#include "def/idlStruct.hpp"
#include "def/idlException.hpp"

/*
 * общее замечание относительно контейнеров
 * Qt для них предоставляет некоторый автомат, ключевые слова QSequentialIterable/QAssociativeIterable, QIterable, QMetaSequence/QMetaAssociation
 * они же в более высокоуровневом виде Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE/Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE (на той же базе)
 *
 * с ними есть некоторые проблемы
 * 1. тип контейнера полностью подменяется внутри движка V4, таким образом теряется возможность передавать этот контейнер обратно в типизированные функции которые ожидают оригинальный тип
 * 2. можно конечно настроить конвертор из QJSValue в этот оригинальный тип, но такой подход открывает новые горизонты проблем
 * 3. "множество" плохо поддерживается
 * 4. ассоциативный так и не удалось запустить, возможно это просто временные баги (только вышла 6.0) и потом они исправятся
 * 5. синтаксический сахар для ассоциативного таки будет не доступен так как ключ там может быть только строкой, а у нас ключ может быть произвольного типа
 *
 * поэтому принимается решение не использовать эти удобняшки вообще, апи реализовать самостоятельно, на сахар JS не завязываться
 */
#include "def/primitivesSet.hpp"
#include "def/primitivesMap.hpp"
#include "def/primitivesList.hpp"
#include "def/primitivesArray.hpp"
#include "def/containerIterator.hpp"

/*
cmt
    future

sbs
    signal
    owner

idl
    interface
    struct
    exception

primitives
    set
    map
    list
    array

    variant
    tuple
    ptr
    ExceptionPtr
    Interface
    int64
    uint64
    byte
    string
    iid
*/
