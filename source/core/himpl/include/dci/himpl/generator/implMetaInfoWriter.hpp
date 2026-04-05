/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "nameMapper.hpp"
#include "../details/metaInfoFetcher.hpp"

#include <cstddef>
#include <type_traits>
#include <iostream>

namespace dci::himpl::generator
{
    template <class... T>
    struct ImplMetaInfoWriter
    {
        template <class... TBases>
        static void exec(){}
    };

    template <class TImpl, class... TOtherImpls>
    struct ImplMetaInfoWriter<TImpl, TOtherImpls...>
    {

        template <class... TBaseCandidates>
        static void exec()
        {
            std::cout<<"    template <> struct ImplMetaInfo< "<<nameMapper<TImpl>()<<" >\n";
            std::cout<<"    {\n";
            std::cout<<"        static constexpr std::size_t _size  = " << details::MetaInfoFetcher<TImpl>::_size << ";\n";
            std::cout<<"        static constexpr std::size_t _align = " << details::MetaInfoFetcher<TImpl>::_align << ";\n";

            std::cout<<"\n";

            std::cout<<"        static constexpr bool _isPolymorphic        = " << (details::MetaInfoFetcher<TImpl>::_isPolymorphic         ? "true" : "false") << ";\n";

            std::cout<<"        static constexpr bool _hasDefaultCtor       = " << (details::MetaInfoFetcher<TImpl>::_hasDefaultCtor        ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasNothrowDefaultCtor= " << (details::MetaInfoFetcher<TImpl>::_hasNothrowDefaultCtor ? "true" : "false") << ";\n";

            std::cout<<"        static constexpr bool _hasCopyCtor          = " << (details::MetaInfoFetcher<TImpl>::_hasCopyCtor           ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasNothrowCopyCtor   = " << (details::MetaInfoFetcher<TImpl>::_hasNothrowCopyCtor    ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasMoveCtor          = " << (details::MetaInfoFetcher<TImpl>::_hasMoveCtor           ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasNothrowMoveCtor   = " << (details::MetaInfoFetcher<TImpl>::_hasNothrowMoveCtor    ? "true" : "false") << ";\n";

            std::cout<<"        static constexpr bool _hasCopyAssign        = " << (details::MetaInfoFetcher<TImpl>::_hasCopyAssign         ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasNothrowCopyAssign = " << (details::MetaInfoFetcher<TImpl>::_hasNothrowCopyAssign  ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasMoveAssign        = " << (details::MetaInfoFetcher<TImpl>::_hasMoveAssign         ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasNothrowMoveAssign = " << (details::MetaInfoFetcher<TImpl>::_hasNothrowMoveAssign  ? "true" : "false") << ";\n";

            //std::cout<<"        static constexpr bool _isAbstract           = " << (details::MetaInfoFetcher<TImpl>::_isAbstract            ? "true" : "false") << ";\n";

            std::cout<<"        static constexpr bool _isFinal              = " << (details::MetaInfoFetcher<TImpl>::_isFinal               ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasDestructor        = " << (details::MetaInfoFetcher<TImpl>::_hasDestructor         ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasNothrowDestructor = " << (details::MetaInfoFetcher<TImpl>::_hasNothrowDestructor  ? "true" : "false") << ";\n";
            std::cout<<"        static constexpr bool _hasDestructorCaller  = " << (details::MetaInfoFetcher<TImpl>::_hasDestructorCaller   ? "true" : "false") << ";\n";

            std::cout<<"\n";

            std::cout<<"        using BasesTuple = std::tuple< ";

            {
                bool first = true;
                auto visitor = [&](auto* fakeBasePointer){
                    using Base = std::remove_pointer_t<decltype(fakeBasePointer)>;

                    if(first)
                    {
                        first = false;
                    }
                    else
                    {
                        std::cout<<", ";
                    }

                    std::cout<<""<<nameMapper<Base>()<<"";
                };

                details::MetaInfoFetcher<TImpl>::template enumerateBases<decltype(visitor), TBaseCandidates...>(visitor);
            }
            std::cout<<" >;\n";

            std::cout<<"\n";

            std::cout<<"        template <class TBase>\n";
            std::cout<<"        static constexpr std::size_t baseOffset() {return ~size_t(0); }\n";

            std::cout<<"    };\n";

            {
                auto visitor = [&](auto* fakeBasePointer){
                    using Base = std::remove_pointer_t<decltype(fakeBasePointer)>;

                    TImpl* d = reinterpret_cast<TImpl *>(alignof(std::max_align_t));
                    Base* b = static_cast<Base *>(d);
                    std::size_t offset = std::size_t(reinterpret_cast<char *>(b) - reinterpret_cast<char *>(d));

                    std::cout<<"    template <> constexpr std::size_t ImplMetaInfo< "<<nameMapper<TImpl>()<<" >::baseOffset< "<<nameMapper<Base>()<<" >() { return "<<offset<<"; }\n";
                };

                details::MetaInfoFetcher<TImpl>::template enumerateBases<decltype(visitor), TBaseCandidates...>(visitor);
            }
            std::cout<<"\n";

            ImplMetaInfoWriter<TOtherImpls...>::template exec<TBaseCandidates...>();
        }
    };
}
