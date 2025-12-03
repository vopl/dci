// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/stiac/link/source.hpp>
#include "impl/source.hpp"

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source::Source(Hub4Source* hub, bytes::Alter&& buffer, bool doFinalization)
        : himpl::FaceLayout<Source, impl::Source>(hub, std::move(buffer), doFinalization)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source::~Source()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hub4Source* Source::context()
    {
        return impl().context();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Source::size() const
    {
        return impl().size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::read(void* data, uint32 size)
    {
        return impl().read(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::read(Bytes& data, uint32 size)
    {
        return impl().read(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::pushPtr(std::type_index typeIndex, const Ptr<void>& v)
    {
        return impl().pushPtr(typeIndex, v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<std::type_index, Ptr<void>> Source::getPtr(uint32 idx)
    {
        return impl().getPtr(idx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid)
    {
        return impl().mapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid)
    {
        return impl().unmapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::finalize()
    {
        return impl().finalize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::finalized() const
    {
        return impl().finalized();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    [[noreturn]] void Source::fail(const char* cszDetails)
    {
        impl().fail(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::emplaceLink(BasePtr&& link, RemoteId id)
    {
        return impl().emplaceLink(std::move(link), id);
    }

}
