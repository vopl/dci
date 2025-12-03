// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../../common.hpp"

namespace dci::qml::qmeta::def::container
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Wrapped, template<class> class... Fronts>
    struct Assembly
        : Wrapped
        , Fronts<Wrapped>...
    {
        using Api = typename TList<typename Fronts<Wrapped>::template Api<Assembly>...>::template Linearize<>;
    };
}
