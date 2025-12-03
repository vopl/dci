// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "user.hpp"
#include "buffer.hpp"

namespace dci::bytes
{
    class Container
    {
    protected:
        Container();
        Container(const Container& from);
        Container(Container&& from);

        ~Container();

        Container& operator=(const Container& from);
        Container& operator=(Container&& from);

    public:
        void resetUsers();

        bytes::Buffer* buffer();
        const bytes::Buffer* buffer() const;
        void setBuffer(bytes::Buffer* buffer);
        void resetBuffer();

    private:
        friend class User;

        void track(User* user);
        void untrack(User* user);

    private:
        bytes::BufferPtr    _buffer;
        User*               _firstUser = nullptr;
    };
}
