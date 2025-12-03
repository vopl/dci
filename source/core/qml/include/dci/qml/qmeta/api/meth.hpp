// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../common.hpp"

namespace dci::qml::qmeta::api
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Name name, auto invoke, Name... paramNames>
    requires
    (
        CallableSignature<decltype(invoke)>::_valid &&
        sizeof...(paramNames)+1 == CallableSignature<decltype(invoke)>::ParamList::_size
    )
    struct Meth
    {
        static constexpr bool       _isMeth {true};
        static constexpr auto       _name {name};
        static constexpr Name<0>    _tag {""};
        static constexpr auto       _invoke {invoke}; // Return (Gadget&, ParamType... paramName)

        using InvokeSignature   = CallableSignature<decltype(invoke)>;
        using ParamTypes        = typename InvokeSignature::ParamList::template PopFront<>::template Map<std::decay_t>;
        using ParamNames        = VList<paramNames...>;
        using Return            = typename InvokeSignature::Return;
        using Strings           = typename VList<_name, _tag>::template Append<ParamNames>;
        using MetaTypes         = typename ParamTypes::template PushFront<Return>;
    };

    template <class T, class=void> struct IsMethImpl: Value<false>{};
    template <class T> struct IsMethImpl<T, std::void_t<decltype(T::_isMeth)>>: Value<T::_isMeth>{};
    template <class T> using IsMeth = Value<IsMethImpl<T>::_v>;
}
