/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "def.hpp"

namespace dci::qml::qmeta
{
    template <class T>
    requires (Def<T>::_declared)
    struct Stringdata
    {
        using GD = Def<T>;

        template <class Semi>
        using GetStrings = typename Semi::Strings;

        using Strings = typename
            VList<GD::_name, Name{""}>::
            template Append<typename GD::Api::template Map<GetStrings>>::
            template Linearize<>::
            template Unique<>;

        static constexpr auto buildData()
        {
            return []<auto... idx>(VList<idx...>)
            {
                constexpr uint headerSize = Strings::_size*2*sizeof(uint);
                constexpr std::size_t bodySize = (0 + ... + (Strings::template Get<idx>::_v._size+1));

                struct Typed
                {
                    std::array<uint, Strings::_size*2> _header;
                    std::array<char, bodySize> _body;
                };

                union Data
                {
                    Typed   _typed{};
                    char    _chars[sizeof(Typed)];
                    uint    _uints[sizeof(Typed::_header)];
                } data{};

                uint pos = 0;
                (...,(
                    (data._typed._header[idx*2+0] = headerSize + pos),
                    (data._typed._header[idx*2+1] = Strings::template Get<idx>::_v._size),
                    (std::copy_n(Strings::template Get<idx>::_v._buf, Strings::template Get<idx>::_v._size, data._typed._body.data()+pos)),
                    (pos += Strings::template Get<idx>::_v._size+1)
                ));

                return data;
            }(MakeSeq<Strings::_size>{});
        }

        static constexpr auto _data{buildData()};

        static constexpr const uint* qt()
        {
            return _data._uints;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <auto name>
        requires (Strings::template _contains<Value<Name{name}>>)
        static constexpr uint _indexFor = Strings::template _index<Value<Name{name}>>;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        static constexpr uint getLen(uint idx)
        {
            return _data._typed._header[idx*2+1];
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        static constexpr const char* getStr(uint idx)
        {
            uint ofs = _data._typed._header[idx*2+0];
            return _data._chars + ofs;
        }
    };
}
