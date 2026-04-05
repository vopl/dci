/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "br.hpp"
#include "ioRoller.hpp"

namespace dci::module::www::http::compress
{
    using namespace std::string_view_literals;

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        struct BufIn
        {
            const uint8_t*  _ptr;
            size_t          _size;
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        struct BufOut
        {
            uint8_t*    _ptr;
            size_t      _size;
        };
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <Direction direction> struct Algo;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<Direction::compress>
        {
            static std::string_view name(){ return "br"sv; }
            static BrotliEncoderState* alloc()
            {
                return BrotliEncoderCreateInstance(
                    [](void* /*opaque*/, size_t size) -> void* { return mm::heap::alloc(size); },
                    [](void* /*opaque*/, void* ptr) -> void { return mm::heap::free(ptr); },
                    nullptr);
            }
            static void free(BrotliEncoderState* state){ return BrotliEncoderDestroyInstance(state); }
            static bool step(BrotliEncoderState* state, BufIn& in, BufOut& out, BrotliEncoderOperation op){ return BrotliEncoderCompressStream(state, op, &in._size, &in._ptr, &out._size, &out._ptr, nullptr); }
            static bool finished(BrotliEncoderState *state){ return BrotliEncoderIsFinished(state); }
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<Direction::decompress>
        {
            static std::string_view name(){ return "unbr"sv; }
            static BrotliDecoderState* alloc()
            {
                return BrotliDecoderCreateInstance(
                    [](void* /*opaque*/, size_t size) -> void* { return mm::heap::alloc(size); },
                    [](void* /*opaque*/, void* ptr) -> void { return mm::heap::free(ptr); },
                    nullptr);
            }
            static void free(BrotliDecoderState* state){ return BrotliDecoderDestroyInstance(state); }
            static BrotliDecoderResult step(BrotliDecoderState* state, BufIn& in, BufOut& out){ return BrotliDecoderDecompressStream(state, &in._size, &in._ptr, &out._size, &out._ptr, nullptr); }
            static const char* error(BrotliDecoderState* state){ return BrotliDecoderErrorString(BrotliDecoderGetErrorCode(state)); }
            static bool finished(BrotliDecoderState *state){ return BrotliDecoderIsFinished(state); }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    Br<direction>::~Br()
    {
        if(_state)
            Algo<direction>::free(_state);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    std::expected<void, ExceptionPtr> Br<direction>::initialize()
    {
        _state = Algo<direction>::alloc();
        if(!_state)
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>(Algo<direction>::name())};
        return {};
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        struct BrIoRoller : IoRoller<BrIoRoller>
        {
            BrIoRoller(Bytes& src, Bytes& dst)
                : IoRoller<BrIoRoller>{src, dst}
            {
            }

            void setIn(const void* ptr, uint32 size)
            {
                _in._ptr = static_cast<const uint8_t*>(ptr);
                _in._size = size;
            }

            uint32 getInSize() const
            {
                return _in._size;
            }

            void setOut(void* ptr, uint32 size)
            {
                _out._ptr = static_cast<uint8_t*>(ptr);;
                _out._size = size;
            }

            uint32 getOutSize() const
            {
                return _out._size;
            }

        public:
            BufIn   _in{};
            BufOut  _out{};
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    std::expected<Bytes, ExceptionPtr> Br<direction>::exec(Bytes&& content, bool finish)
    {
        if(!_state)
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>(Algo<direction>::name())};

        Bytes res;
        {
            BrIoRoller ioRoller{content, res};

            for(;;)
            {
                ioRoller.step();

                if constexpr(Direction::compress == direction)
                {
                    BrotliEncoderOperation op;
                    if(finish && ioRoller.srcAtFinish())
                        op = BROTLI_OPERATION_FINISH;
                    else
                        op = BROTLI_OPERATION_PROCESS;

                    if(!Algo<direction>::step(_state, ioRoller._in, ioRoller._out, op))
                        return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<direction>::name()}+ " failed")};
                }
                else
                {
                    BrotliDecoderResult stepRes = Algo<direction>::step(_state, ioRoller._in, ioRoller._out);
                    if(BROTLI_DECODER_RESULT_ERROR == stepRes)
                        return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<direction>::name()}+ " failed: " + Algo<direction>::error(_state))};

                    if(BROTLI_DECODER_RESULT_SUCCESS == stepRes)
                        break;
                }

                if(ioRoller.srcAtFinish() && !ioRoller.getInSize() && ioRoller.getOutSize()) // пусто на входе и есть место на выходе - это отсутствие прогресса
                    break;
            }
        }

        bool dstFinished = Algo<direction>::finished(_state);

        if(finish && !dstFinished)
            return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<direction>::name()}+ ": incomplete source")};

        if(dstFinished && !content.empty())
            return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<direction>::name()}+ ": extra source")};

        return {std::move(res)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template class Br<Direction::compress>;
    template class Br<Direction::decompress>;
}
