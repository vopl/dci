/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "buffer.hpp"

namespace dci::bytes
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Buffer::Buffer(Chunk* firstChunk, Chunk* lastChunk, uint32 size)
        : _firstChunk(firstChunk)
        , _lastChunk(lastChunk)
        , _size(size)
    {
        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Buffer::~Buffer()
    {
        dbgHeavyAssert(consistent());

        while(_firstChunk)
        {
            delete std::exchange(_firstChunk, _firstChunk->next());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Buffer::shareCounter() const
    {
        return _shareCounter;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Chunk* Buffer::firstChunk()
    {
        dbgHeavyAssert(consistent());
        return _firstChunk;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Chunk* Buffer::lastChunk()
    {
        dbgHeavyAssert(consistent());
        return _lastChunk;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Buffer::size() const
    {
        dbgHeavyAssert(consistent());
        return _size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Buffer::detachData(Chunk*& firstChunk, Chunk*& lastChunk)
    {
        dbgHeavyAssert(consistent());
        firstChunk = std::exchange(_firstChunk, nullptr);
        lastChunk = std::exchange(_lastChunk, nullptr);
        return std::exchange(_size, 0);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Buffer::setFirstChunk(Chunk* chunk)
    {
        _firstChunk = chunk;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Buffer::setLastChunk(Chunk* chunk)
    {
        _lastChunk = chunk;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Buffer::addSize(uint32 size)
    {
        _size += size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Buffer::decSize(uint32 size)
    {
        dbgAssert(size <= _size);
        _size -= size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Buffer::consistent() const
    {
        if(_firstChunk)
        {
            if(!_lastChunk)
            {
                dbgAssert(false);
                return false;
            }

            if(_firstChunk->prev())
            {
                dbgAssert(false);
                return false;
            }

            if(_lastChunk->next())
            {
                dbgAssert(false);
                return false;
            }
        }
        else
        {
            if(_lastChunk)
            {
                dbgAssert(false);
                return false;
            }
        }

        {
            uint32 sizeFromChunks = 0;

            Chunk* c = _firstChunk;
            while(c)
            {
                if(!c->consistent())
                {
                    dbgAssert(false);
                    return false;
                }

                sizeFromChunks += c->size();

                if(!c->next())
                {
                    if(c != _lastChunk)
                    {
                        dbgAssert(false);
                        return false;
                    }
                }

                c = c->next();
            }

            if(sizeFromChunks != _size)
            {
                dbgAssert(false);
                return false;
            }
        }

        {
            uint32 sizeFromChunks = 0;

            Chunk* c = _lastChunk;
            while(c)
            {
                if(!c->consistent())
                {
                    dbgAssert(false);
                    return false;
                }

                sizeFromChunks += c->size();

                if(!c->prev())
                {
                    if(c != _firstChunk)
                    {
                        dbgAssert(false);
                        return false;
                    }
                }

                c = c->prev();
            }

            if(sizeFromChunks != _size)
            {
                dbgAssert(false);
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void intrusive_ptr_add_ref(Buffer* p)
    {
        dbgAssert(p);
        p->_shareCounter++;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void intrusive_ptr_release(Buffer* p)
    {
        dbgAssert(p);
        if(1 == p->_shareCounter)
        {
            delete p;
            return;
        }

        p->_shareCounter--;
    }

}
