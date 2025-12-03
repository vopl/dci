// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "zstd.hpp"
#include "ioRoller.hpp"

namespace dci::module::www::http::compress
{
    using namespace std::string_view_literals;

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <Direction direction> struct Algo;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<Direction::compress>
        {
            static std::string_view name(){ return "zstd"sv; }
            static ZSTD_CCtx* alloc() { return ZSTD_createCCtx(); }
            static size_t free(ZSTD_CCtx* ctx){ return ZSTD_freeCCtx(ctx); }
            static size_t step(ZSTD_CCtx* ctx, ZSTD_outBuffer* out, ZSTD_inBuffer* in, ZSTD_EndDirective end){ return ZSTD_compressStream2(ctx, out, in, end); }
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<Direction::decompress>
        {
            static std::string_view name(){ return "unzstd"sv; }
            static ZSTD_DStream* alloc(){ return ZSTD_createDStream(); }
            static size_t free(ZSTD_DStream* ctx){ return ZSTD_freeDStream(ctx); }
            static size_t step(ZSTD_DStream* ctx, ZSTD_outBuffer* out, ZSTD_inBuffer* in){ return ZSTD_decompressStream(ctx, out, in); }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    Zstd<direction>::~Zstd()
    {
        if(_ctx)
            Algo<direction>::free(_ctx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    std::expected<void, ExceptionPtr> Zstd<direction>::initialize()
    {
        _ctx = Algo<direction>::alloc();
        if(!_ctx)
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>(Algo<direction>::name())};
        return {};
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        struct ZstdIoRoller : IoRoller<ZstdIoRoller>
        {
            ZstdIoRoller(Bytes& src, Bytes& dst)
                : IoRoller<ZstdIoRoller>{src, dst}
            {
            }

            void setIn(const void* ptr, uint32 size)
            {
                _in.src = ptr;
                _in.size = size;
                _in.pos = 0;
            }

            uint32 getInSize() const
            {
                return _in.size - _in.pos;
            }

            void setOut(void* ptr, uint32 size)
            {
                _out.dst = ptr;
                _out.size = size;
                _out.pos = 0;
            }

            uint32 getOutSize() const
            {
                return _out.size - _out.pos;
            }

        public:
            ZSTD_inBuffer   _in{};
            ZSTD_outBuffer  _out{};
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Direction direction>
    std::expected<Bytes, ExceptionPtr> Zstd<direction>::exec(Bytes&& content, bool finish)
    {
        if(!_ctx)
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>(Algo<direction>::name())};

        // bool dstUnflushed{};
        Bytes res;
        {
            ZstdIoRoller ioRoller{content, res};

            for(;;)
            {
                ioRoller.step();

                size_t stepRes;
                if constexpr(Direction::compress == direction)
                {
                    ZSTD_EndDirective end;
                    if(finish && ioRoller.srcAtFinish())
                        end = ZSTD_e_end;
                    else
                        end = ZSTD_e_continue;

                    stepRes = Algo<direction>::step(_ctx, &ioRoller._out, &ioRoller._in, end);
                }
                else
                    stepRes = Algo<direction>::step(_ctx, &ioRoller._out, &ioRoller._in);

                if(ZSTD_isError(stepRes))
                    return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<direction>::name()}+ ": "+ZSTD_getErrorName(stepRes))};

                if(!ioRoller.getInSize() && ioRoller.getOutSize()) // пусто на входе и есть место на выходе - это отсутствие прогресса
                {
                    // dstUnflushed = !!stepRes;
                    break;
                }
            }
        }

        // if(finish && dstUnflushed)
        //     return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<direction>::name()}+ ": incomplete source")};

        return {std::move(res)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template class Zstd<Direction::compress>;
    template class Zstd<Direction::decompress>;
}
