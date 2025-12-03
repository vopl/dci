// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "direction.hpp"

namespace dci::module::www::http::compress
{
    namespace zlib
    {
        enum class Type
        {
            deflate,
            gzip,
        };
    }

    template <zlib::Type type, Direction direction>
    class ZlibSpecialState {};

    template <>
    class ZlibSpecialState<zlib::Type::gzip, Direction::compress>
    {
    protected:
        gz_header _gzh = []
        {
            gz_header gzh{};
            gzh.os = 3; // mimic Unix
            return gzh;
        }();
    };

    template <zlib::Type type, Direction direction>
    class Zlib : ZlibSpecialState<type, direction>
    {
    public:
        ~Zlib();

        std::expected<void, ExceptionPtr> initialize();
        std::expected<Bytes, ExceptionPtr> exec(Bytes&& content, bool finish);

    private:
        z_stream    _strm{};
        bool        _initialized{};
        bool        _dstFinished{};
    };
}
