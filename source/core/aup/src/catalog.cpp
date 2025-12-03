// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/aup/catalog.hpp>
#include "impl/catalog.hpp"

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::Catalog()
        : himpl::FaceLayout<Catalog, impl::Catalog>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::~Catalog()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::deserialize(Bytes&& blob)
    {
        return impl().deserialize(std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Catalog::serialize()
    {
        return impl().serialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<Oid> Catalog::enumerate(catalog::Object::Type type)
    {
        return impl().enumerate(type);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Catalog::put(catalog::ObjectPtr&& object)
    {
        return impl().put(std::move(object));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Catalog::has(const Oid& oid)
    {
        return impl().has(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    catalog::ObjectPtr Catalog::get(const Oid& oid)
    {
        return impl().get(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::del(const Oid& oid)
    {
        return impl().del(oid);
    }
}
