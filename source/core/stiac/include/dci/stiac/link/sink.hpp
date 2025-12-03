// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include <dci/stiac/implMetaInfo.hpp>
#include <dci/himpl.hpp>
#include <dci/primitives.hpp>
#include <dci/bytes.hpp>
#include "id.hpp"
#include "base.hpp"
#include "hub4Sink.hpp"

namespace dci::stiac::link
{
    class Sink final
        : public himpl::FaceLayout<Sink, impl::Sink>
    {
    private:
        Sink() = delete;
        Sink(const Sink&) = delete;
        void operator=(const Sink&) = delete;
        void operator=(Sink&&) = delete;

    public:
        API_DCI_STIAC Sink(Hub4Sink* hub, bytes::Alter&& buffer, bool doFinalization = true);
        API_DCI_STIAC Sink(Sink&& from);
        API_DCI_STIAC ~Sink();

        API_DCI_STIAC Hub4Sink* context();

        void API_DCI_STIAC write(const void* data, uint32 size);
        void API_DCI_STIAC write(Bytes&& data);

        std::pair<uint32, bool> API_DCI_STIAC mapTuid(const std::array<uint8, 16>& tuid);

        std::pair<uint32, bool> API_DCI_STIAC pushPtr(void* ptrMark);

        void API_DCI_STIAC finalize();

        void API_DCI_STIAC fail(const char* cszDetails);

        LocalId API_DCI_STIAC emplaceLink(BasePtr&& link);

        Sink& operator<<(auto&& v);
    };
}
