// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/idl/contract/lidRegistry.hpp>
#include "impl/lidRegistry.hpp"

namespace dci::idl::contract
{
    LidRegistry::LidRegistry()
        : himpl::FaceLayout<LidRegistry, impl::LidRegistry>{}
    {
    }

    LidRegistry::~LidRegistry()
    {
    }

    Lid LidRegistry::emplace(const Id& id)
    {
        return impl().emplace(id);
    }

    Lid LidRegistry::get(const Id& id) const
    {
        return impl().get(id);
    }

    const Id& LidRegistry::get(Lid lid) const
    {
        return impl().get(lid);
    }

    namespace
    {
        class LidRegistryCreator : public LidRegistry
        {
        } lidRegistryInstance;
    }

    LidRegistry& lidRegistry API_DCI_IDL = lidRegistryInstance;
}
