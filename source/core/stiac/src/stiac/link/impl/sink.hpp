// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/stiac/link/id.hpp>
#include <dci/stiac/link/base.hpp>
#include <dci/primitives.hpp>
#include <dci/bytes.hpp>
#include <map>

namespace dci::stiac::link
{
    class Hub4Sink;
}

namespace dci::stiac::link::impl
{
    class Sink final
    {

    public:
        Sink(Hub4Sink* hub, bytes::Alter&& dataAlter, bool doFinalization);
        Sink(Sink&& from);
        ~Sink();

        Hub4Sink* context();

        void write(const void* data, uint32 size);
        void write(Bytes&& data);

        std::pair<uint32, bool> mapTuid(const std::array<uint8, 16>& tuid);

        std::pair<uint32, bool> pushPtr(void* ptrMark);

        void finalize();

        void fail(const char* cszDetails);

        LocalId emplaceLink(BasePtr&& link);

    private:
        Hub4Sink *                  _hub;

        bytes::Alter                _dataAlter;
        bool                        _doFinalization;
        bool                        _finalized;

        std::map<void *, uint32>    _ptrShareContext;
    };
}
