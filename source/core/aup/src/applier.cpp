// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/aup/applier.hpp>
#include <dci/aup/catalog.hpp>
#include <dci/aup/storage.hpp>
#include "impl/applier.hpp"

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Applier::Applier()
        : himpl::FaceLayout<Applier, impl::Applier>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Applier::~Applier()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::addCatalog(Catalog* c)
    {
        return impl().addCatalog(himpl::face2Impl(c));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::addStorage(Storage* s)
    {
        return impl().addStorage(himpl::face2Impl(s));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::addRoot(const Oid& oid, const Set<catalog::File::Kind>& fileKinds)
    {
        return impl().addRoot(oid, fileKinds);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    applier::Result Applier::process(const String& place, applier::Task task)
    {
        return impl().process(place, task);
    }
}
