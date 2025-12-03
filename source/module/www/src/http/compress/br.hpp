// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "direction.hpp"
#include <brotli/decode.h>
#include <brotli/encode.h>

namespace dci::module::www::http::compress
{
    template <Direction direction>
    class Br
    {
    public:
        ~Br();

        std::expected<void, ExceptionPtr> initialize();
        std::expected<Bytes, ExceptionPtr> exec(Bytes&& content, bool finish);

    private:
        using State = utils::ct::If<
            Direction::compress == direction,
            BrotliEncoderState,
            BrotliDecoderState
        >*;

        State _state{};
    };
}
