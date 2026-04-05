/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "streamCipher.hpp"
#include <cstdlib>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher(const StreamCipher&)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher(StreamCipher&&)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::~StreamCipher()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher& StreamCipher::operator=(const StreamCipher&)
    {
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher& StreamCipher::operator=(StreamCipher&&)
    {
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::setKey(const void* key, std::size_t len)
    {
        (void)key;
        (void)len;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::setIv(const void* iv, std::size_t len)
    {
        (void)iv;
        (void)len;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::cipher(const void* in, void* out, std::size_t len)
    {
        (void)in;
        (void)out;
        (void)len;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::seek(std::uint64_t offset)
    {
        (void)offset;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::clear()
    {
        dbgWarn("must be overrided!");
        abort();
    }
}
