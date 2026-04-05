/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <array>
#include <tuple>
#include <QMetaType>
#include <dci/utils/ct.hpp>
#include <dci/sbs/signal.hpp>
#include <dci/idl/introspection.hpp>
#include <dci/cmt/future.hpp>

namespace dci::qml::qmeta
{
    using namespace utils::ct;
    using namespace idl::introspection;
}

namespace dci::qml::qmeta
{
    template<std::size_t size>
    struct Name
    {
        static constexpr std::size_t _size = size;
        char _buf[_size + 1];
        constexpr Name(const Name<size>& n)
        {
            for(std::size_t i = 0; i <= _size; ++i) _buf[i] = n._buf[i];
        }
        constexpr Name(const char* s)
        {
            for(std::size_t i = 0; i <= _size; ++i) _buf[i] = s[i];
        }
        constexpr Name(std::array<char, size+1> s)
        {
            for(std::size_t i = 0; i <= _size; ++i) _buf[i] = s[i];
        }
    };
    template<std::size_t size> Name(const Name<size>&) -> Name<size>;
    template<std::size_t N> Name(char const (&)[N]) -> Name<N - 1>;
    template<std::size_t N> Name(std::array<char, N>) -> Name<N - 1>;


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class=void>
    struct CallableSignature
    {
        static constexpr bool _valid = false;
        using Return = void;
        using ParamList = TList<>;
    };

    template <class R, class... Params>
    struct CallableSignature<R(*)(Params...)>
    {
        static constexpr bool _valid = true;
        using Return = R;
        using ParamList = TList<Params...>;
    };

    template <class C, class R, class... Params>
    struct CallableSignature<R(C::*)(Params...)>
    {
        static constexpr bool _valid = true;
        using Return = R;
        using ParamList = TList<Params...>;
    };

    template <class C, class R, class... Params>
    struct CallableSignature<R(C::*)(Params...) const>
    {
        static constexpr bool _valid = true;
        using Return = R;
        using ParamList = TList<Params...>;
    };

    template <class C>
    struct CallableSignature<C, std::void_t<decltype(&C::operator())>>
        : CallableSignature<decltype(&C::operator())>
    {
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C>
    requires (CallableSignature<C>::ParamList::_size == 1)
    bool registerConvertorIfMissing(C&& convertor)
    {
        using Signature = CallableSignature<std::decay_t<C>>;
        using From = typename std::decay_t<typename Signature::ParamList::template Get<0>>;
        using To = typename std::decay_t<typename Signature::Return>;

        if(!QMetaType::hasRegisteredConverterFunction<From, To>())
        {
            return QMetaType::registerConverter<From, To>(std::forward<C>(convertor));
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MV>
    requires (CallableSignature<MV>::ParamList::_size == 1)
    bool registerMutableViewIfMissing(MV&& mutableView)
    {
        using Signature = CallableSignature<std::decay_t<MV>>;
        using From = typename std::decay_t<typename Signature::ParamList::template Get<0>>;
        using To = typename std::decay_t<typename Signature::Return>;

        if(!QMetaType::hasRegisteredMutableViewFunction<From, To>())
        {
            return QMetaType::registerMutableView<From, To>(std::forward<MV>(mutableView));
        }

        return false;
    }
}
