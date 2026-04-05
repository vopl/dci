/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <string>
#include <cstring>
#include <cstdint>

namespace dci::idl::im
{
    class Sign final
    {
    public:
        static constexpr std::size_t _size = 16;

    public:
        Sign();
        Sign(const std::uint8_t (&data)[_size]);
        Sign(const Sign& from);
        Sign(Sign&& from);
        ~Sign();

        Sign& operator=(const Sign& from);

        template <std::size_t N>
        Sign& operator=(const std::array<std::uint8_t, N>& from) requires (N >= _size);

        std::uint8_t* data();
        const std::uint8_t* data() const;

        std::string toHex(std::size_t charStart=0, std::size_t chars=_size*2) const;
        bool fromHex(const std::string& txt);
        void fromRnd();

        bool operator<(const Sign& with) const;
        bool operator>(const Sign& with) const;
        bool operator<=(const Sign& with) const;
        bool operator>=(const Sign& with) const;
        bool operator==(const Sign& with) const;
        bool operator!=(const Sign& with) const;

    private:
        std::uint8_t _data[_size];
    };

    Sign operator^(const Sign& a, const Sign& b);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t N>
    Sign& Sign::operator=(const std::array<std::uint8_t, N>& from) requires (N >= _size)
    {
        std::memcpy(_data, from.data(), _size);
        return *this;
    }
}
