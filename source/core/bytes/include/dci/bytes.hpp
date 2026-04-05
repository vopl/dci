/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "api.hpp"
#include <dci/bytes/implMetaInfo.hpp>
#include <dci/himpl.hpp>
#include "bytes/chunk.hpp"
#include "bytes/alter.hpp"
#include <dci/primitives.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <compare>

namespace dci
{
    namespace bytes::details
    {
        struct CszWrapper
        {
            const char * _csz;
            CszWrapper(const char* csz) : _csz(csz) {}
        };
    }

    class API_DCI_BYTES Bytes
        : public himpl::FaceLayout<Bytes, impl::Bytes>
    {
    public:
        Bytes();
        Bytes(const Bytes& from);
        Bytes(Bytes&& from);
        Bytes(bytes::Chunk* first, bytes::Chunk* last, uint32 size);
        Bytes(const void* data, uint32 size);
        explicit Bytes(bytes::details::CszWrapper cszWrapper);

        template<std::size_t sizeWithZero>
        Bytes(const char (&data)[sizeWithZero]);

        template<class T, std::size_t size> requires(sizeof(byte) == sizeof(T))
        Bytes(const std::array<T, size>& data);

        template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
        Bytes(const std::basic_string<T, OtherArgs...>& data);

        template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
        Bytes(const std::basic_string_view<T, OtherArgs...>& data);

        template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
        Bytes(const std::vector<T, OtherArgs...>& data);

        template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
        Bytes(const std::deque<T, OtherArgs...>& data);

        ~Bytes();

        Bytes& operator=(const Bytes& from);
        Bytes& operator=(Bytes&& from);

        bool operator==(const Bytes& with) const;
        bool operator!=(const Bytes& with) const;
        bool operator<(const Bytes& with) const;
        bool operator>(const Bytes& with) const;
        bool operator<=(const Bytes& with) const;
        bool operator>=(const Bytes& with) const;
        std::strong_ordering operator<=>(const Bytes& with) const;

        bool empty() const;
        uint32 size() const;

        String toString() const;
        String toHex() const;

        void clear();

        bytes::Alter begin();
        bytes::Cursor begin() const;
        bytes::Cursor cbegin() const;

        bytes::Alter end();
        bytes::Cursor end() const;
        bytes::Cursor cend() const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<std::size_t sizeWithZero>
    Bytes::Bytes(const char (&data)[sizeWithZero])
        : Bytes(data, static_cast<uint32>(sizeWithZero-1))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T, std::size_t N> requires(sizeof(byte) == sizeof(T))
    Bytes::Bytes(const std::array<T, N>& data)
        : Bytes(data.data(), static_cast<uint32>(data.size()))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
    Bytes::Bytes(const std::basic_string<T, OtherArgs...>& data)
        : Bytes(data.data(), static_cast<uint32>(data.size()))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
    Bytes::Bytes(const std::basic_string_view<T, OtherArgs...>& data)
        : Bytes(data.data(), static_cast<uint32>(data.size()))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
    Bytes::Bytes(const std::vector<T, OtherArgs...>& data)
        : Bytes(data.data(), static_cast<uint32>(data.size()))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class... OtherArgs> requires(sizeof(byte) == sizeof(T))
    Bytes::Bytes(const std::deque<T, OtherArgs...>& data)
        : Bytes(data.data(), static_cast<uint32>(data.size()))
    {
        dbgFatal(!"по итераторам");
    }
}
