// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include "../api/prop.hpp"
#include <dci/idl/introspection.hpp>
#include <QMetaType>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T_>
    requires (idl::introspection::isStruct<T_>)
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
                [](const T& s){ return fieldValue<T, FI>(s); },
                [](T& s, const fieldType<T, FI>& f){ fieldValue<T, FI>(s) = f; }
            >;
        };

        template <class MI>
        using GetFieldApi = typename GetFieldApiImpl<MI::_v>::Result;
        using ApiSelf = typename MakeSeq<fieldsCount<T>>::template Map<GetFieldApi>;

        template <class BI>
        using GetBaseApi = typename Def<baseType<T, BI::_v>>::Api;
        using ApiBases = typename MakeSeq<basesCount<T>>::template Map<GetBaseApi>::template Linearize<>;

        using Api = typename ApiSelf::template Append<ApiBases>;
    };
}
