// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::http::inputSlicer
{
    class SourceAdapter
    {
    public:
        SourceAdapter(bytes::Alter& data);

        class Null;
        class ForHdr;
        class ForBody;

        bool empty() const;

        Null& null();
        ForHdr& forHdr();
        ForBody& forBody();

    public:
        class Null
        {
        };

        class ForHdr
        {
        public:
            ForHdr(bytes::Alter& data);
            ~ForHdr();

            const char* segmentBegin() const;
            const char* segmentEnd() const;
            std::size_t segmentSize() const;

            bool empty() const;
            char front();

            void dropFront(std::size_t amount);

        private:
            bytes::Alter&       _data;
            std::string_view    _segment;
            std::size_t         _dropped{};
        };

        class ForBody
        {
        public:
            ForBody(bytes::Alter& data);

            bool empty() const;
            Bytes detach(uint32 maxSize = ~uint32{});

        private:
            bytes::Alter& _data;
        };

    private:
        bytes::Alter&                   _data;
        Variant<Null, ForHdr, ForBody>  _var;
    };
}
