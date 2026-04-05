/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
