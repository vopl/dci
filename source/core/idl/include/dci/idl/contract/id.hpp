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
#include <dci/utils/integer.hpp>
#include <dci/primitives.hpp>

namespace dci::idl::contract
{
    struct Id
        : Array<uint8, 16>
    {
        static constexpr uint32 _size = 16;

        template <class Int>
        constexpr Array<Int, 16/sizeof(Int)> asArray() const requires(std::is_integral_v<Int>);

        bool API_DCI_IDL fromHex(const String& hex);
        String API_DCI_IDL toHex(uint32 chars = _size*2) const;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Int>
    inline constexpr Array<Int, 16/sizeof(Int)> Id::asArray() const requires(std::is_integral_v<Int>)
    {
        using Res = Array<Int, _size/sizeof(Int)>;
        Res res {};

        for(std::size_t i{}; i<_size; ++i)
        {
            res[i/sizeof(Int)] |= Int{operator[](i)} << (8 * (i%sizeof(Int)));
        }

        return res;
    }
}
