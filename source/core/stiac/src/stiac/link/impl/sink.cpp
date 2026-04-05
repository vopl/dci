/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "sink.hpp"
#include <dci/stiac/link/sink.hpp>
#include <dci/logger.hpp>
#include <cstring>

namespace dci::stiac::link::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sink::Sink(Hub4Sink* hub, bytes::Alter&& dataAlter, bool doFinalization)
        : _hub(hub)
        , _dataAlter(std::move(dataAlter))
        , _doFinalization(doFinalization)
        , _finalized(false)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sink::Sink(Sink&& from)
        : _hub(std::exchange(from._hub, nullptr))
        , _dataAlter(std::move(from._dataAlter))
        , _doFinalization(std::exchange(from._doFinalization, false))
        , _finalized(std::exchange(from._finalized, true))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sink::~Sink()
    {
        finalize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hub4Sink* Sink::context()
    {
        return _hub;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::write(const void* data, uint32 size)
    {
        _dataAlter.write(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::write(Bytes&& data)
    {
        _dataAlter.write(std::move(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<uint32, bool> Sink::mapTuid(const std::array<uint8, 16>& tuid)
    {
        dbgAssert(_hub);
        return _hub->mapTuid(tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<std::uint32_t, bool> Sink::pushPtr(void* ptrMark)
    {
        auto iterAndPresense = _ptrShareContext.try_emplace(ptrMark, _ptrShareContext.size());
        return std::make_pair(iterAndPresense.first->second, iterAndPresense.second);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::finalize()
    {
        if(_doFinalization && !_finalized)
        {
            _finalized = true;

            dbgAssert(_hub);
            _hub->finalize(
                        himpl::impl2Face<link::Sink>(*this),
                        std::move(_dataAlter));

            //_dataAlter.reset();
            _ptrShareContext.clear();

            dbgAssert(!(_dataAlter.size() + _dataAlter.sizeBack()));
            dbgAssert(_ptrShareContext.empty());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sink::fail(const char* cszDetails)
    {
        LOGW("stiac failed during output making: " << cszDetails << ". Fail will be ignored.");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    LocalId Sink::emplaceLink(link::BasePtr&& link)
    {
        dbgAssert(_hub);
        return _hub->emplaceLink(std::move(link));
    }

}
