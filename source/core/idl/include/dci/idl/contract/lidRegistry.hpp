// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include <dci/himpl.hpp>
#include <dci/idl/implMetaInfo.hpp>
#include "id.hpp"
#include "lid.hpp"

namespace dci::idl::contract
{
    class API_DCI_IDL LidRegistry
        : public himpl::FaceLayout<LidRegistry, impl::LidRegistry>
    {
    protected:
        LidRegistry();
        ~LidRegistry();

    public:
        Lid emplace(const Id& id);
        Lid get(const Id& id) const;
        const Id& get(Lid lid) const;
    };

    extern LidRegistry& lidRegistry API_DCI_IDL;
}
