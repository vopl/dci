// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "direction.hpp"
#include <zstd.h>

namespace dci::module::www::http::compress
{
    template <Direction direction>
    class Zstd
    {
    public:
        ~Zstd();

        std::expected<void, ExceptionPtr> initialize();
        std::expected<Bytes, ExceptionPtr> exec(Bytes&& content, bool finish);

    private:
        using Ctx = utils::ct::If<
            Direction::compress == direction,
            ZSTD_CCtx,
            ZSTD_DCtx
        >*;

        Ctx _ctx{};
    };
}
