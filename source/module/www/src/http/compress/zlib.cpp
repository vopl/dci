// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "zlib.hpp"
#include "ioRoller.hpp"

namespace dci::module::www::http::compress
{
    using namespace std::string_view_literals;

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <zlib::Type type, Direction direction> struct Algo;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<zlib::Type::deflate, Direction::compress>
        {
            static std::string_view name(){ return "deflate"sv; }
            static int init(z_stream* strm){ return deflateInit2(strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY); }
            static int end(z_stream* strm){ return deflateEnd(strm); }
            static int step(z_stream* strm, int flush){ return deflate(strm, flush); }
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<zlib::Type::deflate, Direction::decompress>
        {
            static std::string_view name(){ return "inflate"sv; }
            static int init(z_stream* strm){ return inflateInit2(strm, -MAX_WBITS); }
            static int end(z_stream* strm){ return inflateEnd(strm); }
            static int step(z_stream* strm, int flush){ return inflate(strm, flush); }
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<zlib::Type::gzip, Direction::compress>
        {
            static std::string_view name(){ return "gzip"sv; }
            static int init(z_stream* strm, gz_headerp gzh)
            {
                if(int i = deflateInit2(strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS+16 , MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY); Z_OK != i)
                    return i;
                return deflateSetHeader(strm, gzh);
            }
            static int end(z_stream* strm){ return deflateEnd(strm); }
            static int step(z_stream* strm, int flush){ return deflate(strm, flush); }
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <> struct Algo<zlib::Type::gzip, Direction::decompress>
        {
            static std::string_view name(){ return "gunzip"sv; }
            static int init(z_stream* strm){ return inflateInit2(strm, MAX_WBITS+16); }
            static int end(z_stream* strm){ return inflateEnd(strm); }
            static int step(z_stream* strm, int flush){ return inflate(strm, flush); }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <zlib::Type type, Direction direction>
    Zlib<type, direction>::~Zlib()
    {
        if(_initialized)
            Algo<type, direction>::end(&_strm);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <zlib::Type type, Direction direction>
    std::expected<void, ExceptionPtr> Zlib<type, direction>::initialize()
    {
        dbgAssert(!_initialized);

        _strm.zalloc = [](voidpf /*opaque*/, unsigned items, unsigned size){ return mm::heap::alloc(items*size); };
        _strm.zfree = [](voidpf /*opaque*/, voidpf address){ return mm::heap::free(address); };
        //_strm.opaque = this;

        int initRes;
        if constexpr(zlib::Type::gzip == type && Direction::compress == direction)
            initRes = Algo<type, direction>::init(&_strm, &this->_gzh);
        else
            initRes = Algo<type, direction>::init(&_strm);
        if(Z_OK != initRes)
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>(Algo<type, direction>::name())};

        dbgAssert(!_strm.msg);
        _initialized = true;
        return {};
    }

    namespace
    {
        struct ZlibIoRollerStreamHolder
        {
            z_stream& _strm;
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        struct ZlibIoRoller
            : ZlibIoRollerStreamHolder
            , IoRoller<ZlibIoRoller>
        {
            ZlibIoRoller(z_stream& strm, Bytes& src, Bytes& dst)
                : ZlibIoRollerStreamHolder{strm}
                , IoRoller<ZlibIoRoller>{src, dst}
            {
            }

            void setIn(const void* ptr, uint32 size)
            {
                _strm.next_in = const_cast<z_const Bytef *>(static_cast<const Bytef *>(ptr));
                _strm.avail_in = size;
            }

            uint32 getInSize() const
            {
                return _strm.avail_in;
            }

            void setOut(void* ptr, uint32 size)
            {
                _strm.next_out = static_cast<Bytef *>(ptr);
                _strm.avail_out = size;
            }

            uint32 getOutSize() const
            {
                return _strm.avail_out;
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <zlib::Type type, Direction direction>
    std::expected<Bytes, ExceptionPtr> Zlib<type, direction>::exec(Bytes&& content, bool finish)
    {
        if(!_initialized)
            return std::unexpected{exception::buildInstance<api::http::error::InternalError>(Algo<type, direction>::name())};

        if(_strm.msg)
            return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<type, direction>::name()}+ " bad state: " + _strm.msg)};

        if(_dstFinished)
        {
            if(content.empty())
                return {};
            return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<type, direction>::name()}+ ": extra source")};
        }

        Bytes res;
        {
            ZlibIoRoller ioRoller{_strm, content, res};
            bool bufError{};
            while(!_dstFinished && !bufError)
            {
                ioRoller.step();

                int flushMode;
                if(finish && ioRoller.srcAtFinish())
                    flushMode = Direction::compress == direction ? Z_FINISH : Z_SYNC_FLUSH;
                else
                    flushMode = Z_NO_FLUSH;

                int i = Algo<type, direction>::step(&_strm, flushMode);

                switch(i)
                {
                case Z_OK:
                    break;
                case Z_BUF_ERROR:
                    bufError = true;
                    _strm.msg = {};
                    break;
                case Z_STREAM_END:
                    _dstFinished = true;
                    break;
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_STREAM_ERROR:
                case Z_MEM_ERROR:
                default:
                    _dstFinished = true;

                    if(_strm.msg)
                        return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<type, direction>::name()}+ ": " + zError(i)+", " + _strm.msg)};

                    return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<type, direction>::name()}+ ": " + zError(i))};
                }
            }
        }

        if(!_dstFinished && finish)
            return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<type, direction>::name()}+ ": incomplete source")};

        if(_dstFinished && !content.empty())
            return std::unexpected{exception::buildInstance<api::http::error::CompressionFailed>(std::string{Algo<type, direction>::name()}+ ": extra source")};

        return {std::move(res)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template class Zlib<zlib::Type::deflate, Direction::compress>;
    template class Zlib<zlib::Type::deflate, Direction::decompress>;
    template class Zlib<zlib::Type::gzip, Direction::compress>;
    template class Zlib<zlib::Type::gzip, Direction::decompress>;
}
