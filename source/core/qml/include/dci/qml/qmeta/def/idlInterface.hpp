// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
