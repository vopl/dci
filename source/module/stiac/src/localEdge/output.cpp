/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "output.hpp"
#include "../localEdge.hpp"

namespace dci::module::stiac::localEdge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Output::Output(Bytes &data)
        : _data{ data }
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Output::~Output()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::Sink Output::makeSink(uint32 reserveIfCan)
    {
        dbgAssert(!_hasActiveSink);
        _hasActiveSink = true;

        dbgAssert(!_reserved);

        bytes::Alter a(_data.end());
        if(reserveIfCan && !a.sizeBack())
        {
            _reserved = reserveIfCan;
            a.advance(static_cast<int32>(reserveIfCan));
        }

        return link::Sink(this, std::move(a));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Output::finalize(link::Sink& sink, bytes::Alter&& buffer)
    {
        dbgAssert(_hasActiveSink);
        _hasActiveSink = false;

        (void)sink;

        if(_reserved)
        {
            dbgAssert(_reserved <= buffer.sizeBack());
            bytes::Alter{std::move(buffer)};
            _data.begin().remove(_reserved);
            _reserved = 0;
        }
        else
        {
            bytes::Alter{std::move(buffer)};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<uint32, bool> Output::mapTuid(const std::array<uint8, 16>& tuid)
    {
        auto res = _tuidMap.try_emplace(tuid, uint32(_tuidMap.size()));
        return std::make_pair(res.first->second, res.second);
    }
}
