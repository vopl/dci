// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "accumuler.hpp"
#include "../compress/unified.hpp"

namespace dci::module::www::http::inputSlicer::state
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct RequestNull
    {
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct RequestFirstLine
    {
        Accumuler<std::array<char, 32>> _method;
        Accumuler<std::string, 8192>    _uri;
        Accumuler<std::array<char, 16>> _version;

        std::optional<api::http::firstLine::Method>     _parsedMethod;
        std::optional<api::http::firstLine::Version>    _parsedVersion;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct ResponseNull
    {
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct ResponseFirstLine
    {
        Accumuler<std::array<char, 16>> _version;
        std::uint16_t                   _statusCode{};
        std::uint16_t                   _statusCodeCharsCount{};
        Accumuler<std::string, 64>      _statusText;

        std::optional<api::http::firstLine::Version>    _parsedVersion;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct Headers
    {
        struct Current
        {
            Accumuler<std::array<char, 64>> _key;
            Accumuler<std::string, 8192>    _value;

            enum class Kind
            {
                unknown,
                regular,
                valueContinue,
                empty
            } _kind{};

            void reset();
        } _current;

        struct Сonveyor
        {
            std::size_t                         _totalHeadersCount{};
            std::size_t                         _totalValueSize{};
            primitives::List<api::http::Header> _tail;
            bool                                _allowLastValueContinue{};

            bool canDetachSome() const;
            primitives::List<api::http::Header> detachSome();

        } _conveyor;

        struct BodyRelated
        {
            enum class Portionality
            {
                null,
                untilClose,
                byContentLength,
                chunked,
            } _portionality{};

            uint64 _contentLength{};

            api::http::message::tunable::Compression _compression{};

            Set<String> _trailers;
        } _bodyRelated;
    };
    constexpr std::size_t _maxEntityHeadersCount{256}; // VS Headers::_conveyor._totalHeadersCount
    constexpr std::size_t _maxEntityHeaderValueSize{32768}; // VS Headers::_conveyor._totalValueSize

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct Body
    {
        using Decompressor = compress::Unified<compress::Direction::decompress>;
        Decompressor _decompressor;

        Bytes _content;
        Set<String> _trailers;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct BodyUntilClose : Body
    {
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct BodyByContentLength : Body
    {
        uint64 _contentLength{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct BodyChunked : Body
    {
        enum State
        {
            null,
            length,
            LF0,
            content,
            CR1, LF1,
            done
        } _state{};
        uint32 _length{};
        uint32 _length2Eat{};
    };
}
