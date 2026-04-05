/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "bitIndex.hpp"
#include "vm.hpp"
#include "utils/sized_cast.hpp"
#include "utils/align.hpp"
#include <dci/utils/compiler.hpp>
#include <dci/utils/dbg.hpp>

#include <cstdlib>

namespace dci::mm::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    BitIndex<volume>::BitIndex()
    {
        if(!vm::protect(this, Config::_pageSize, vm::Protection::rw))
        {
            dbgWarn("unable to protect region");
            std::abort();
        }
        _header._protectedSize = Config::_pageSize;
        _header._maxAllocatedAddress = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    BitIndex<volume>::~BitIndex()
    {
        if(!vm::protect(this, sizeof(*this), vm::Protection::none))
        {
            dbgWarn("unable to protect region");
            std::abort();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    bitIndex::Address BitIndex<volume>::allocate()
    {
        bitIndex::Address addr = _topLevel.allocate();
        if(unlikely(bitIndex::_badAddress == addr || volume <= addr))
        {
            return bitIndex::_badAddress;
        }

        if(addr > _header._maxAllocatedAddress)
        {
            updateProtection(addr);
        }

        return addr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    bool BitIndex<volume>::isAllocated(bitIndex::Address address)
    {
        if(_header._maxAllocatedAddress < address)
        {
            return false;
        }

        return _topLevel.isAllocated(address);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    void BitIndex<volume>::deallocate(bitIndex::Address address)
    {
        dbgAssert(_header._maxAllocatedAddress >= address);
        _topLevel.deallocate(address);

        if(_header._maxAllocatedAddress == address)
        {
            updateProtection(_topLevel.maxAllocatedAddress());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    void BitIndex<volume>::updateProtection(bitIndex::Address addr)
    {
        _header._maxAllocatedAddress = addr;

        std::size_t requiredArea = _topLevel.requiredAreaForAddress(addr) + offsetof(BitIndex<volume>, _topLevel);

        updateProtection(utils::sized_cast<char *>(this) + requiredArea);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t volume>
    void BitIndex<volume>::updateProtection(void* addr)
    {
        dbgAssert(addr > this && addr < utils::sized_cast<char *>(this) + utils::alignUp(sizeof(*this), Config::_pageSize));
        std::size_t protectedSize = static_cast<std::size_t>(static_cast<char *>(addr) - utils::sized_cast<char *>(this)) / Config::_pageSize * Config::_pageSize + Config::_pageSize*2;

        if(protectedSize > _header._protectedSize)
        {
            if(!vm::protect(
                        utils::sized_cast<char *>(this) + _header._protectedSize,
                        protectedSize - _header._protectedSize,
                        vm::Protection::rw))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }
            _header._protectedSize = protectedSize;
        }
        else if(protectedSize < _header._protectedSize - Config::_pageSize)
        {
            if(!vm::protect(
                        utils::sized_cast<char *>(this) + protectedSize,
                        _header._protectedSize - protectedSize,
                        vm::Protection::none))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }
            _header._protectedSize = protectedSize;
        }
    }
}
