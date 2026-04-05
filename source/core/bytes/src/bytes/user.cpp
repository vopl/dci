/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "user.hpp"
#include "container.hpp"

namespace dci::bytes
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User::User()
    {
        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User::User(const User& from)
        : _container(from._container)
        , _chunk(from._chunk)
        , _chunkPos(from._chunkPos)
        , _pos(from._pos)
    {
        if(_container)
        {
            _container->track(this);
        }

        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User::User(User&& from)
        : _container(std::exchange(from._container, nullptr))
        , _chunk(std::exchange(from._chunk, nullptr))
        , _chunkPos(std::exchange(from._chunkPos, 0))
        , _pos(std::exchange(from._pos, 0))
    {
        if(_container)
        {
            _container->untrack(&from);
            _container->track(this);
        }

        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User::User(Container* container, bool posAtBegin)
        : _container(container)
    {
        dbgAssert(_container);
        _container->track(this);

        if(_container->buffer())
        {
            if(posAtBegin)
            {
                _chunk = _container->buffer()->firstChunk();
                _chunkPos = 0;
                _pos = 0;
            }
            else
            {
                _chunk = _container->buffer()->lastChunk();
                _pos = _container->buffer()->size();
                if(_chunk)
                {
                    _chunkPos = _chunk->size();
                }
                else
                {
                    _chunkPos = 0;
                    dbgAssert(!_pos);
                }
            }
        }
        else
        {
            _chunk = nullptr;
            _chunkPos = 0;
            _pos = 0;
        }

        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User::~User()
    {
        reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User& User::operator=(const User& from)
    {
        dbgHeavyAssert(consistent());

        if(_container)
        {
            _container->untrack(this);
        }

        _container = from._container;

        if(_container)
        {
            _container->track(this);
        }
        else
        {
            _next = _prev = nullptr;
        }

        _chunk = from._chunk;
        _chunkPos = from._chunkPos;
        _pos = from._pos;

        dbgHeavyAssert(consistent());

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    User& User::operator=(User&& from)
    {
        dbgHeavyAssert(consistent());
        dbgHeavyAssert(from.consistent());

        if(_container)
        {
            _container->untrack(this);
        }

        _container = std::exchange(from._container, nullptr);

        _chunk = std::exchange(from._chunk, nullptr);
        _chunkPos = std::exchange(from._chunkPos, 0);
        _pos = std::exchange(from._pos, 0);

        if(_container)
        {
            _container->untrack(&from);
            _container->track(this);
        }
        else
        {
            _next = _prev = nullptr;
        }

        dbgHeavyAssert(consistent());
        dbgHeavyAssert(from.consistent());

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void User::reset()
    {
        if(_container)
        {
            _container->untrack(this);
            _container = nullptr;
        }

        dbgAssert(!_next);
        dbgAssert(!_prev);
        dbgAssert(!_container);

        if(_chunk)
        {
            _chunk = nullptr;
            _chunkPos = 0;
            _pos = 0;
        }

        dbgAssert(!_chunk);
        dbgAssert(!_chunkPos);
        dbgAssert(!_pos);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool User::consistent() const
    {
        if(_container && _container->buffer())
        {
            if(!_container->buffer()->consistent())
            {
                dbgAssert(false);
                return false;
            }

            if(_pos > _container->buffer()->size())
            {
                dbgAssert(false);
                return false;
            }

            if(_chunk)
            {
                if(!_chunk->consistent())
                {
                    dbgAssert(false);
                    return false;
                }

                if(_chunkPos > _chunk->size())
                {
                    dbgAssert(false);
                    return false;
                }

                {
                    uint32 size = _chunk->size() - _chunkPos;
                    Chunk* c = _chunk->next();
                    while(c)
                    {
                        if(!c->consistent())
                        {
                            dbgAssert(false);
                            return false;
                        }

                        size += c->size();
                        c = c->next();
                    }

                    if(_pos + size != _container->buffer()->size())
                    {
                        dbgAssert(false);
                        return false;
                    }
                }

                {
                    uint32 sizeBack = _chunkPos;
                    Chunk* c = _chunk->prev();
                    while(c)
                    {
                        if(!c->consistent())
                        {
                            dbgAssert(false);
                            return false;
                        }

                        sizeBack += c->size();
                        c = c->prev();
                    }

                    if(_pos != sizeBack)
                    {
                        dbgAssert(false);
                        return false;
                    }
                }
            }
            else
            {
                if(_pos)
                {
                    dbgAssert(false);
                    return false;
                }

                if(_chunkPos)
                {
                    dbgAssert(false);
                    return false;
                }
            }

            return true;
        }

        if(_chunk)
        {
            dbgAssert(false);
            return false;
        }

        if(_chunkPos)
        {
            dbgAssert(false);
            return false;
        }

        if(_pos)
        {
            dbgAssert(false);
            return false;
        }

        return true;

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void User::resetOtherUsersInContainer()
    {
        while(_next)
        {
            _next->reset();
        }

        while(_prev)
        {
            _prev->reset();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void User::reassignContainer(Container* container)
    {
        dbgAssert(_container);
        dbgAssert(container);

        dbgAssert(_container->buffer() == container->buffer() || !_container->buffer());

        _container->untrack(this);
        _container = container;
        _container->track(this);

        dbgHeavyAssert(consistent());
    }
}
