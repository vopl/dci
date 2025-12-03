// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/stiac/link/id.hpp>
#include <dci/stiac/link/base.hpp>
#include <dci/primitives.hpp>
#include <dci/bytes.hpp>
#include <typeindex>
#include <vector>
#include <utility>

namespace dci::stiac::link
{
    class Hub4Source;
}

namespace dci::stiac::link::impl
{
    class Source final
    {

    public:
        Source(Hub4Source* hub, bytes::Alter&& dataAlter, bool doFinalization);
        ~Source();

        Hub4Source* context();

    public:
        uint32 size() const;
        void read(void* data, uint32 size);
        void read(Bytes& data, uint32 size);
        void pushPtr(std::type_index typeIndex, const Ptr<void>& v);
        std::pair<std::type_index, Ptr<void>> getPtr(uint32 idx);
        bool mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid);
        bool unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid);
        void finalize();
        bool finalized() const;

        [[noreturn]] void fail(const char* cszDetails);

        bool emplaceLink(BasePtr&& link, RemoteId id);

    private:
        Hub4Source *            _hub;

        bytes::Alter            _dataAlter;

        using PtrValue = std::pair<
            std::type_index,
            Ptr<void>
        >;

        std::vector<PtrValue>   _ptrShareContext;

        bool                    _doFinalization;
        bool                    _finalized;
    };
}
