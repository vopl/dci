// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "hmDescriptorBase.hpp"
#include "id.hpp"
#include "lid.hpp"

namespace dci::idl::contract
{
    struct MdState;

    struct MdDescriptorBase
    {
        using FCreateMds        = MdState *(*)();
        using FDisconnectWires  = void (*)(MdState* mds);
        using FDestroyMds       = void (*)(MdState* mds);
        using FResolveHmd       = const HmDescriptorBase *(*)(const Id& id, const Lid& lid);

        const FCreateMds        _createMds;
        const FDisconnectWires  _disconnectWires;
        const FDestroyMds       _destroyMds;
        const FResolveHmd       _resolveHmd;

        const Id&   _id;
        const Lid   _lid;

        constexpr MdDescriptorBase(
                const FCreateMds  createMds,
                const FDisconnectWires disconnectWires,
                const FDestroyMds destroyMds,
                const FResolveHmd resolveHmd,
                const Id& id,
                const Lid& lid)
            : _createMds{createMds}
            , _disconnectWires{disconnectWires}
            , _destroyMds{destroyMds}
            , _resolveHmd{resolveHmd}
            , _id{id}
            , _lid{lid}
        {
        }
    };
}
