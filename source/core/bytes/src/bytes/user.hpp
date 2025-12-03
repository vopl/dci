// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include <dci/bytes/chunk.hpp>

namespace dci::bytes
{
    class Container;
    class User
    {
    public:
        User();
        User(const User& from);
        User(User&& from);
        User(Container* container, bool posAtBegin);
        virtual ~User();

        User& operator=(const User& from);
        User& operator=(User&& from);

        virtual void reset();

    protected:
        bool consistent() const;
        void resetOtherUsersInContainer();

    private:
        friend class Container;

        void reassignContainer(Container* container);

        //интрузивность в рамках конейнера
        User *      _next = nullptr;
        User *      _prev = nullptr;

    protected:
        Container * _container = nullptr;

        Chunk *     _chunk = nullptr;
        uint16      _chunkPos = 0;
        uint32      _pos = 0;
    };
}
