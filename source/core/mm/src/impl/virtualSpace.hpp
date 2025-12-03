// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "config.hpp"
#include "bitIndex.hpp"
#include "utils/align.hpp"

#include "stack/content.hpp"

namespace dci::mm::impl
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
    class VirtualSpace
    {

    public:
        VirtualSpace();
        ~VirtualSpace();

        static VirtualSpace& single();

    public:
        stack::Content* allocStackContent();
        void freeStackContent(stack::Content* stackContent);
        void setupPanicHandler(void(*)(int));

        ////////////////////////////////////////////////////////////////
        bool vmAccessHandler(void* addr);
        void vmPanic(int signum);

    private:
        using StacksBitIndex = BitIndex<Config::_stacksAmount>;

        static constexpr std::size_t _stacksBitIndexAlignedSize = utils::alignUp(sizeof(StacksBitIndex), Config::_pageSize);
        static constexpr std::size_t _stacksPad = Config::_stackPages*Config::_pageSize;
        static constexpr std::size_t _stackSize = Config::_stackPages*Config::_pageSize;
        static constexpr std::size_t _stacksAlignedSize = Config::_stacksAmount * _stackSize;

        static constexpr std::size_t _vmSize =
                _stacksBitIndexAlignedSize + _stacksPad + _stacksAlignedSize;


        void* _vm;

        StacksBitIndex* _stacksBitIndex;
        void* _stacks;
        void(*_panic)(int){};
    };
}
