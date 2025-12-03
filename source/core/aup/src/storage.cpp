// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/aup/storage.hpp>
#include "impl/storage.hpp"

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::Storage()
        : himpl::FaceLayout<Storage, impl::Storage>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::~Storage()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::reset(const std::string& place, bool autoFixIfCan)
    {
        return impl().reset(place, autoFixIfCan);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<Oid> Storage::enumerate()
    {
        return impl().enumerate();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const std::string& localPath, Bytes&& blob)
    {
        return impl().put(localPath, std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has(const std::string& localPath)
    {
        return impl().has(localPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get(const std::string& localPath, uint32 from, uint32 to)
    {
        return impl().get(localPath, from, to);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del(const std::string& localPath)
    {
        return impl().del(localPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const Oid& oid, Bytes&& blob)
    {
        return impl().put(oid, std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has(const Oid& oid)
    {
        return impl().has(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get(const Oid& oid, uint32 from, uint32 to)
    {
        return impl().get(oid, from, to);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del(const Oid& oid)
    {
        return impl().del(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::delAll(bool andPlaceDirectory)
    {
        return impl().delAll(andPlaceDirectory);
    }
}
