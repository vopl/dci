/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "mac.hpp"
#include <utility>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(std::size_t digestSize)
        : Hash{digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(const Mac& from)
        : Hash{from}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::Mac(Mac&& from)
        : Hash{std::move(from)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac::~Mac()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac& Mac::operator=(const Mac& from)
    {
        Hash::operator=(from);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mac& Mac::operator=(Mac&& from)
    {
        Hash::operator=(std::move(from));
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mac::setKey(const void* key, std::size_t len)
    {
        (void)key;
        (void)len;
        //ok
    }
}
