// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
