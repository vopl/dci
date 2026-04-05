/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "virtualSpace.hpp"
#include "vm.hpp"

#include "stack/content.hpp"
#include "utils/sized_cast.ipp"
#include "utils/align.hpp"
#include "bitIndex.ipp"
#include "bitIndex/level.ipp"

#include <new>
#include <cstdlib>

namespace dci::mm::impl
{
    namespace
    {
        bool g_vmAccessHandler(void* addr)
        {
            return VirtualSpace::single().vmAccessHandler(addr);
        }
        void g_vmPanic(int signum)
        {
            return VirtualSpace::single().vmPanic(signum);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    VirtualSpace::VirtualSpace()
    {
        _vm = vm::alloc(_vmSize);

        if(!_vm)
        {
            std::fprintf(stderr, "unable to allocate vm\n");
            std::fflush(stderr);
            std::abort();
        }

        std::size_t addr = utils::sized_cast<std::size_t>(_vm);

        addr = utils::alignUp(addr, Config::_pageSize);
        _stacksBitIndex = new(utils::sized_cast<void *>(addr)) StacksBitIndex;
        addr += _stacksBitIndexAlignedSize;

        addr = utils::alignUp(addr, Config::_stackPages*Config::_pageSize);
        _stacks = utils::sized_cast<void *>(addr);
        //addr += _stacksAlignedSize;

        if(!vm::init(g_vmAccessHandler, g_vmPanic))
        {
            std::fprintf(stderr, "unable to initialize vm\n");
            std::fflush(stderr);
            std::abort();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    VirtualSpace::~VirtualSpace()
    {
        if(_stacksBitIndex)
        {
            _stacksBitIndex->~BitIndex();
            _stacksBitIndex = nullptr;
        }
        _stacks = nullptr;

        dbgAssert(_vm);
        vm::free(_vm, _vmSize);
        _vm = nullptr;

        vm::deinit(&g_vmAccessHandler);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        union VirtualSpaceArea
        {
            char _area{};
            VirtualSpace _virtualSpace;
            VirtualSpaceArea() : _virtualSpace{} {}
            ~VirtualSpaceArea() {}
        } g_virtualSpaceArea{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    VirtualSpace& VirtualSpace::single()
    {
        return g_virtualSpaceArea._virtualSpace;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    stack::Content* VirtualSpace::allocStackContent()
    {
        bitIndex::Address stackBitAddr = _stacksBitIndex->allocate();

        if(bitIndex::_badAddress == stackBitAddr)
        {
            dbgWarn("no more stacks available");

            std::fprintf(stderr, "unable to allocate new stack, no space available\n");
            std::fflush(stderr);
            std::abort();
        }

        stack::Content* stackContent = utils::sized_cast<stack::Content *>(_stacks) + stackBitAddr;

        new(stackContent) stack::Content;

        return stackContent;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void VirtualSpace::freeStackContent(stack::Content* stackContent)
    {
        bitIndex::Address stackBitAddr = static_cast<bitIndex::Address>(stackContent - utils::sized_cast<stack::Content *>(_stacks));

        dbgAssert(_stacksBitIndex->isAllocated(stackBitAddr));
        _stacksBitIndex->deallocate(stackBitAddr);

        stackContent->~Content();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void VirtualSpace::setupPanicHandler(void(* panic)(int))
    {
        _panic = panic;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool VirtualSpace::vmAccessHandler(void* ptr)
    {

        if(likely(utils::sized_cast<std::uintptr_t>(ptr) - utils::sized_cast<std::uintptr_t>(_stacks) < _stacksAlignedSize))
        {
            bitIndex::Address stackBitAddr = static_cast<bitIndex::Address>(static_cast<std::size_t>(utils::sized_cast<char *>(ptr) - utils::sized_cast<char *>(_stacks)) / _stackSize);
            dbgAssert(_stacksBitIndex->isAllocated(stackBitAddr));
            (void)stackBitAddr;

            void* stackContentPtr = utils::sized_cast<void *>(utils::sized_cast<std::uintptr_t>(ptr) / _stackSize * _stackSize);

            stack::Content* stackContent = utils::sized_cast<stack::Content *>(stackContentPtr);

            std::uintptr_t offset = utils::sized_cast<std::uintptr_t>(ptr) - utils::sized_cast<std::uintptr_t>(stackContent);

            return stackContent->vmAccessHandler(offset);
        }

        return false;
    }

    void VirtualSpace::vmPanic(int signum)
    {
        if(_panic)
        {
            return _panic(signum);
        }
    }
}
