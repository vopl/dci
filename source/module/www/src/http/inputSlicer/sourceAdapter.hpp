/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
