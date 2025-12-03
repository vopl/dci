// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "outputBase.hpp"
#include "../utils.hpp"
#include "../enumSupport.hpp"
#include <string_view>

namespace dci::module::www::http
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    OutputBase<Support, Impl, Api>::OutputBase(Support* support, Api&& api)
        : Base{support, std::move(api)}
    {
        using namespace std::literals;

        // in headers(list<Header>, bool done);
        this->_api.methods()->headers() += this->_sol * [this](const primitives::List<api::http::Header>& headers, bool done)
        {
            {
                bytes::Alter out{this->_buffer.end()};

                {
                    auto compressionBySettings = [&](api::http::message::tunable::Compression compression) -> std::expected<void, ExceptionPtr>
                    {
                        if(api::http::message::tunable::Compression::byHeaders == compression || api::http::message::tunable::Compression::none == compression)
                            return {};
                        return _compressor.initialize(compression);
                    };

                    std::expected<void, ExceptionPtr> res = compressionBySettings(this->_contentCompression);
                    if(res)
                        res = compressionBySettings(this->_transferCompression);
                    if(!res)
                    {
                        this->_support->failed(static_cast<Impl*>(this), static_cast<Impl*>(this)->makeExceptionCompressionFailed(std::move(res).error()));
                        this->_support->close({});
                        return;
                    }
                }
                {
                    if(api::http::message::tunable::Encoding::chunked == this->_transferEncoding)
                        _chunked = true;
                }

                for(const api::http::Header& header : headers)
                {
                    bool success = header.key.visit([&](const auto& value) -> bool
                    {
                        using K = std::decay_t<decltype(value)>;
                        if constexpr(std::is_same_v<api::http::header::KeyRecognized, K>)
                        {
                            if( (api::http::message::tunable::Compression::byHeaders == this->_contentCompression  && api::http::header::KeyRecognized::Content_Encoding  == value) ||
                                (api::http::message::tunable::Compression::byHeaders == this->_transferCompression && api::http::header::KeyRecognized::Transfer_Encoding == value))
                            {
                                bool someBadValue = false;
                                ExceptionPtr compressionInitializeError;
                                utils::split(header.value, ", "sv, [&](std::string_view part)
                                {
                                    if("chunked"sv == part)
                                        _chunked = true;
                                    else
                                    {
                                        std::optional<api::http::message::tunable::Compression> parsed = enumSupport::toEnum<api::http::message::tunable::Compression>(part);
                                        if(parsed)
                                        {
                                            std::expected<void, ExceptionPtr> res = _compressor.initialize(*parsed);
                                            if(!res)
                                            {
                                                someBadValue = true;
                                                compressionInitializeError = std::move(res).error();
                                            }
                                        }
                                        else
                                            someBadValue = true;
                                    }
                                });

                                if(someBadValue)
                                {
                                    ExceptionPtr error = compressionInitializeError ?
                                        static_cast<Impl*>(this)->makeExceptionCompressionFailed(std::move(compressionInitializeError)) :
                                        static_cast<Impl*>(this)->makeExceptionAmbiguousHeaders();

                                    this->_support->failed(static_cast<Impl*>(this), std::move(error));
                                    this->_support->close({});
                                    return false;
                                }
                            }

                            if( (api::http::message::tunable::Encoding::byHeaders == this->_transferEncoding && api::http::header::KeyRecognized::Transfer_Encoding == value))
                            {
                                utils::split(header.value, ", "sv, [&](std::string_view part)
                                {
                                    if("chunked"sv == part)
                                        _chunked = true;
                                });
                            }

                            std::optional<std::string_view> optStr = enumSupport::toString(value);
                            if(!optStr)
                            {
                                this->_support->failed(static_cast<Impl*>(this), static_cast<Impl*>(this)->makeExceptionAmbiguousHeaders());
                                this->_support->close({});
                                return false;
                            }
                            out.write(optStr->data(), optStr->size());
                        }
                        else
                            out.write(value.data(), value.size());
                        return true;
                    });
                    if(!success)
                        return;

                    out.write(": ");
                    out.write(header.value.data(), header.value.size());
                    out.write("\r\n");
                }

                if(done)
                    out.write("\r\n");
            }

            if(done)
                this->flushBuffer();
        };

        // in data(bytes, bool done);
        this->_api.methods()->data() += this->_sol * [this](Bytes&& data, bool done)
        {
            auto processChunked = [&](Bytes&& data)
            {
                bytes::Alter dst = this->_buffer.end();
                if(_chunked)
                {
                    if(data.empty())
                    {
                        if(done)
                            dst.write("0\r\n\r\n");
                    }
                    else
                    {
                        std::array<char, 32> numStr;
                        const char* numEnd = std::to_chars(std::begin(numStr), std::end(numStr), data.size(), 16).ptr;
                        dst.write(numStr.data(), numEnd - numStr.begin());
                        dst.write("\r\n");
                        dst.write(std::move(data));
                        if(done)
                            dst.write("\r\n0\r\n\r\n");
                        else
                            dst.write("\r\n");
                    }
                }
                else
                    dst.write(std::move(data));
            };

            if(_compressor.imbued())
            {
                std::expected<Bytes, ExceptionPtr> compressed = _compressor.exec(std::move(data), done);
                if(!compressed)
                {
                    this->_support->failed(static_cast<Impl*>(this), static_cast<Impl*>(this)->makeExceptionCompressionFailed(std::move(compressed).error()));
                    this->_support->close({});
                    return;
                }
                processChunked(std::move(compressed).value());
            }
            else
                processChunked(std::move(data));

            this->flushBuffer();
        };

        // in done();
        this->_api.methods()->done() += this->_sol * [this]()
        {
            this->flushBuffer();
            this->apiDone();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    OutputBase<Support, Impl, Api>::~OutputBase()
    {
    }
}
