// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "none.hpp"
#include "zlib.hpp"
#include "br.hpp"
#include "zstd.hpp"
#include "www.hpp"

namespace dci::module::www::http::compress
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    class Unified
    {
    public:
        std::expected<void, ExceptionPtr> initialize(api::http::message::tunable::Compression type);
        bool imbued() const;
        std::expected<Bytes, ExceptionPtr> exec(Bytes&& content, bool finish);

    private:
        using State = dci::primitives::Variant
        <
            None,
            Zlib<compress::zlib::Type::deflate,   direction>,
            Zlib<compress::zlib::Type::gzip,      direction>,
            Br  <                                 direction>,
            Zstd<                                 direction>
        >;
        State       _state;
        std::size_t _processedSource{};
        std::size_t _processedDestination{};
    };
}
