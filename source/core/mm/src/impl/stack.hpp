// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include <cstddef>
#include "stack/content.hpp"

namespace dci::mm::impl
{
    class Stack final
    {
    public:
        Stack();
        Stack(Stack&& from);
        ~Stack();

        Stack& operator=(Stack&& from);

        void initialize();
        bool initialized() const;

    public:
        bool growsDown() const;
        bool hasGuard() const;

        char* begin() const;
        char* end() const;
        std::size_t size() const;

        void compact();

    private:
        stack::Content* _content;
    };
}
