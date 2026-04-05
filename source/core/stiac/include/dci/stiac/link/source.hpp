/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../api.hpp"
#include <dci/stiac/implMetaInfo.hpp>
#include <dci/himpl.hpp>
#include <dci/primitives.hpp>
#include <dci/bytes.hpp>
#include "id.hpp"
#include "base.hpp"
#include "hub4Source.hpp"
#include "source/fail.hpp"
#include <typeindex>

namespace dci::stiac::link
{
    template <class T> class SourceZombie;

    class Source
        : public himpl::FaceLayout<Source, impl::Source>
    {
    private:
        Source() = delete;
        Source(const Source&) = delete;
        Source(Source&&) = delete;
        void operator=(const Source&) = delete;
        void operator=(Source&&) = delete;

    public:
        API_DCI_STIAC Source(Hub4Source* hub, bytes::Alter&& buffer, bool doFinalization = true);
        API_DCI_STIAC ~Source();

        API_DCI_STIAC Hub4Source* context();

    public:
        uint32 API_DCI_STIAC size() const;
        void API_DCI_STIAC read(void* data, uint32 size);
        void API_DCI_STIAC read(Bytes& data, uint32 size);
        void API_DCI_STIAC pushPtr(std::type_index typeIndex, const Ptr<void>& v);
        std::pair<std::type_index, Ptr<void>> API_DCI_STIAC getPtr(uint32 idx);
        bool API_DCI_STIAC mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid);
        bool API_DCI_STIAC unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid);
        void API_DCI_STIAC finalize();
        bool API_DCI_STIAC finalized() const;

        [[noreturn]] void API_DCI_STIAC fail(const char* cszDetails);

        bool API_DCI_STIAC emplaceLink(BasePtr&& link, RemoteId id);

        Source& operator>>(auto&& v);

        template <class T>
        SourceZombie<T> makeZombie(const T& v);
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    class SourceZombie
    {
    public:
        SourceZombie(Source* source, const T& v);

    public:
        void pushPtr(std::type_index typeIndex, Ptr<void> v);
        std::pair<std::type_index, Ptr<void>> getPtr(uint32 idx);
        bool mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid);
        bool unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid);

        void fail(const char* cszDetails);

        bool emplaceLink(BasePtr&& link, RemoteId id);

        SourceZombie& operator>>(T& v);
        SourceZombie& operator>>(auto&& v);

    private:
        Source* _source;
        T _v;
    };

}
