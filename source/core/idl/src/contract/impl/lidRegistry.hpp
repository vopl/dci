// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/idl/contract/id.hpp>
#include <dci/idl/contract/lid.hpp>
#include <dci/primitives.hpp>

namespace dci::idl::contract::impl
{
    class LidRegistry final
    {
    public:
        LidRegistry();
        ~LidRegistry();

    public:
        Lid emplace(const Id& id);
        Lid get(const Id& id) const;
        const Id& get(Lid lid) const;

    private:
        Map<Id, Lid>    _fwd;
        Map<Lid, Id>    _bwd;
        Lid             _lidGen;
    };
}
