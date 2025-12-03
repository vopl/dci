// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/stiac/link/sink.hpp>
#include "impl/sink.hpp"

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sink::Sink(Hub4Sink* hub, bytes::Alter&& buffer, bool doFinalization)
        : himpl::FaceLayout<Sink, impl::Sink>(hub, std::move(buffer), doFinalization)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sink::Sink(Sink&& from)
        : himpl::FaceLayout<Sink, impl::Sink>(himpl::face2Impl(std::move(from)))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sink::~Sink()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hub4Sink* Sink::context()
    {
        return impl().context();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::write(const void* data, uint32 size)
    {
        return impl().write(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::write(Bytes&& data)
    {
        return impl().write(std::move(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<uint32, bool> Sink::mapTuid(const std::array<uint8, 16>& tuid)
    {
        return impl().mapTuid(tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<uint32, bool> Sink::pushPtr(void* ptrMark)
    {
        return impl().pushPtr(ptrMark);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::finalize()
    {
        return impl().finalize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::fail(const char* cszDetails)
    {
        return impl().fail(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    LocalId Sink::emplaceLink(BasePtr&& link)
    {
        return impl().emplaceLink(std::move(link));
    }

}
