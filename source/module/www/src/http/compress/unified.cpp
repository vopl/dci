/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "unified.hpp"

namespace dci::module::www::http::compress
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    std::expected<void, ExceptionPtr> Unified<direction>::initialize(api::http::message::tunable::Compression type)
    {
        if(!_state.template holds<None>())
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>()};

        switch(type)
        {
        case api::http::message::tunable::Compression::none:
            return {};
        case api::http::message::tunable::Compression::deflate:
            return _state.template emplace<Zlib<compress::zlib::Type::deflate, direction>>().initialize();
        case api::http::message::tunable::Compression::gzip:
            return _state.template emplace<Zlib<compress::zlib::Type::gzip, direction>>().initialize();
        case api::http::message::tunable::Compression::br:
            return _state.template emplace<Br<direction>>().initialize();
        case api::http::message::tunable::Compression::zstd:
            return _state.template emplace<Zstd<direction>>().initialize();
        default:
            break;
        }

        return std::unexpected{exception::buildInstance<api::http::error::InternalError>()};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    bool Unified<direction>::imbued() const
    {
        return !_state.template holds<compress::None>();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    std::expected<Bytes, ExceptionPtr> Unified<direction>::exec(Bytes&& content, bool finish)
    {
        dbgAssert(!content.empty() || finish);
        _processedSource += content.size();
        if(finish && !_processedSource)
            return {};

        std::expected<Bytes, ExceptionPtr> result = _state.visit([&](auto& concrete)
        {
            return concrete.exec(std::move(content), finish);
        });

        if(result.has_value())
            _processedDestination += result.value().size();
        return result;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template class Unified<Direction::compress>;
    template class Unified<Direction::decompress>;
}
