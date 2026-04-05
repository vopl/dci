/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "alter.hpp"
#include "../../impl/bytes.hpp"
#include "../performing/cursorPromotor.hpp"
#include "../performing/partCursorPromotor.hpp"
#include "../performing/alterPromotor.hpp"
#include "../performing/arrayPromotor.hpp"
#include "../performing/stepResult.hpp"
#include "../performing/execute.hpp"

namespace dci::bytes::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter::Alter()
        : Cursor()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter::Alter(const Alter& from)
        : Cursor(from)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter::Alter(Alter&& from)
        : Cursor(static_cast<Cursor&&>(from))
        , _preparedWriteBuffer(std::move(from._preparedWriteBuffer))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter::Alter(Bytes* container, bool posAtBegin)
        : Cursor(container, posAtBegin)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter::~Alter()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter& Alter::operator=(const Alter& from)
    {
        if(this != &from)
        {
            _preparedWriteBuffer.reset();
            Cursor::operator=(from);
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter& Alter::operator=(Alter&& from)
    {
        if(this != &from)
        {
            _preparedWriteBuffer = std::move(from._preparedWriteBuffer);
            Cursor::operator=(static_cast<Cursor&&>(from));
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Alter::reset()
    {
        _preparedWriteBuffer.reset();
        Cursor::reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Alter::advance(int32 ioffset)
    {
        dbgHeavyAssert(consistent());

        if(!_container && ioffset)
        {
            dbgWarn("unable to advance missing container");
            abort();
            return;
        }

        ioffset -= Cursor::advance(ioffset);

        dbgHeavyAssert(consistent());

        if(ioffset > 0)
        {
            //fwd

            copyBufferBecauseOfWrite();
            uint32 offset = static_cast<uint32>(ioffset);

            _pos += offset;

            if(!_chunk)
            {
                _chunkPos = static_cast<uint16>(std::min(offset, Chunk::bufferSize()));
                _chunk = new Chunk{0, _chunkPos};
                _container->setBuffer(new Buffer{_chunk, _chunk, _chunkPos});
                offset -= _chunkPos;
            }

            _container->buffer()->addSize(offset);

            uint32 space = Chunk::bufferSize() - _chunkPos - _chunk->begin();
            if(space > 0)
            {
                if(space >= offset)
                {
                    uint32 lack = offset - (_chunk->size() - _chunkPos);
                    _chunk->setEnd(static_cast<uint16>(_chunk->end() + lack));
                    _chunkPos += lack;

                    dbgHeavyAssert(consistent());
                    return;
                }
                else
                {
                    _chunk->setEnd(Chunk::bufferSize());
                    offset -= space;
                }
            }

            while(offset > Chunk::bufferSize())
            {
                _chunk->setNext(new Chunk{nullptr, _chunk, 0, Chunk::bufferSize()});
                _chunk = _chunk->next();
                offset -= Chunk::bufferSize();
            }

            dbgAssert(offset <= Chunk::bufferSize());
            if(offset)
            {
                _chunk->setNext(new Chunk{nullptr, _chunk, 0, static_cast<uint16>(offset)});
                _chunk = _chunk->next();
                _chunkPos = static_cast<uint16>(offset);
            }
            else
            {
                _chunkPos = _chunk->size();
            }

            _container->buffer()->setLastChunk(_chunk);
        }
        else if(ioffset < 0)
        {
            //bwd
            copyBufferBecauseOfWrite();
            uint32 offset = static_cast<uint32>(-ioffset);

            dbgAssert(!_chunk || !_chunk->prev());
            dbgAssert(0 == _chunkPos);
            dbgAssert(0 == _pos);

            if(!_chunk)
            {
                if(offset <= Chunk::bufferSize())
                {
                    _chunk = new Chunk{static_cast<uint16>(Chunk::bufferSize()-offset), Chunk::bufferSize()};
                    _container->setBuffer(new Buffer{_chunk, _chunk, offset});
                    dbgHeavyAssert(consistent());
                    return;
                }

                _chunk = new Chunk{0, Chunk::bufferSize()};
                _container->setBuffer(new Buffer{_chunk, _chunk, Chunk::bufferSize()});
                offset -= Chunk::bufferSize();
            }

            _container->buffer()->addSize(offset);

            if(_chunk->begin())
            {
                if(_chunk->begin() >= offset)
                {
                    _chunk->setBegin(static_cast<uint16>(_chunk->begin() - offset));
                    dbgHeavyAssert(consistent());
                    return;
                }
                else
                {
                    offset -= _chunk->begin();
                    _chunk->setBegin(0);
                }
            }

            while(offset > Chunk::bufferSize())
            {
                _chunk->setPrev(new Chunk{_chunk, nullptr, 0, Chunk::bufferSize()});
                _chunk = _chunk->prev();
                offset -= Chunk::bufferSize();
            }

            dbgAssert(offset <= Chunk::bufferSize());
            if(offset)
            {
                _chunk->setPrev(new Chunk{_chunk, nullptr, static_cast<uint16>(Chunk::bufferSize()-offset), Chunk::bufferSize()});
                _chunk = _chunk->prev();
            }

            _container->buffer()->setFirstChunk(_chunk);
        }

        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void* Alter::continuousData4Write()
    {
        dbgHeavyAssert(consistent());

        if(_chunk && _chunk->size() > _chunkPos)
        {
            copyBufferBecauseOfWrite();
            return _chunk->data() + _chunkPos;
        }

        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    byte* Alter::prepareWriteBuffer(/*out*/uint32& size)
    {
        dbgHeavyAssert(consistent());

        if(_preparedWriteBuffer)
        {
            //уже подготовлен выделенный
            dbgAssert(!_preparedWriteBuffer->begin());
            dbgAssert(!_preparedWriteBuffer->size());
            return _preparedWriteBuffer->buffer();
        }

        if(!_container)
        {
            size = 0;
            return nullptr;
        }

        if(!_chunk)
        {
            _preparedWriteBuffer.reset(new Chunk);
            size = Chunk::bufferSize();
            return _preparedWriteBuffer->buffer();
        }

        for(;;)
        {
            if(!_chunk->next())
            {
                //для последнего особая логика, ему можно двинуть конец

                uint16 absChunkPos = _chunk->begin() + _chunkPos;
                if(absChunkPos < Chunk::bufferSize())
                {
                    //есть место
                    size = Chunk::bufferSize() - absChunkPos;
                    return _chunk->data() + _chunkPos;
                }

                //нет места
                _preparedWriteBuffer.reset(new Chunk);
                size = Chunk::bufferSize();
                return _preparedWriteBuffer->buffer();
            }

            //очередной блок
            if(_chunk->size() > _chunkPos)
            {
                size = _chunk->size() - _chunkPos;
                return _chunk->data() + _chunkPos;
            }

            _chunk = _chunk->next();
            _chunkPos = 0;
        }

        dbgWarn("never here");
        size = 0;
        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Alter::commitWriteBuffer(uint32 size)
    {
        dbgHeavyAssert(consistent());
        dbgAssert(size <= Chunk::bufferSize());

        if(!size)
        {
            _preparedWriteBuffer.reset();
            return;
        }

        if(_preparedWriteBuffer)
        {
            dbgAssert(!_preparedWriteBuffer->begin());
            dbgAssert(!_preparedWriteBuffer->size());
            _preparedWriteBuffer->setEnd(static_cast<uint16>(size));

            if(_chunk)
            {
                dbgAssert(_chunkPos >= _chunk->size());
                dbgAssert(!_chunk->next());

                _chunk->setNext(_preparedWriteBuffer.release());
                _chunk->next()->setPrev(_chunk);
                _chunk = _chunk->next();
                _chunkPos = static_cast<uint16>(size);
                _pos += size;

                dbgAssert(_container->buffer());
                _container->buffer()->setLastChunk(_chunk);
                _container->buffer()->addSize(size);

                dbgHeavyAssert(consistent());
                return;
            }

            _chunk = _preparedWriteBuffer.release();
            _chunkPos = static_cast<uint16>(size);
            dbgAssert(!_pos);
            _pos += size;

            if(_container->buffer())
            {
                dbgAssert(!_container->buffer()->firstChunk());
                dbgAssert(!_container->buffer()->lastChunk());
                dbgAssert(!_container->buffer()->size());

                dbgAssert(!_chunk->prev());
                dbgAssert(!_chunk->next());
                dbgAssert(_chunk->size() == size);

                _container->buffer()->setFirstChunk(_chunk);
                _container->buffer()->setLastChunk(_chunk);
                _container->buffer()->addSize(size);
            }
            else
                _container->setBuffer(new Buffer{_chunk, _chunk, size});

            dbgHeavyAssert(consistent());
            return;
        }

        dbgAssert(_chunk);

        uint32 tailSizeAlready = _chunk->size() - _chunkPos;
        if(tailSizeAlready >= size)
        {
            _chunkPos += size;
            _pos += size;

            dbgHeavyAssert(consistent());
            return;
        }

        _container->buffer()->addSize(size - tailSizeAlready);
        _chunkPos += size;
        _pos += size;
        _chunk->setEnd(_chunk->begin() + _chunkPos);

        dbgHeavyAssert(consistent());
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Alter::write(const void* src, uint32 size)
    {
        writeImpl(performing::ArrayPromotor(src, size));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::write(const char* srcz)
    {
        return writeImpl(performing::ArrayPromotor<const void>(srcz, static_cast<uint32>(::strlen(srcz))));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::write(const Bytes& src, uint32 maxSize)
    {
        Cursor c(src.begin());
        return writeImpl(performing::PartCursorPromotor(c, maxSize));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::write(Bytes&& src, uint32 maxSize)
    {
        if(src.buffer() && src.buffer()->shareCounter() > 1)
        {
            //COW источника, за счет write(const Bytes& src...
            return write(src, maxSize);
        }

        dbgHeavyAssert(consistent());

        if(src.empty() || !maxSize)
        {
            return 0;
        }

        _preparedWriteBuffer.reset();
        copyBufferBecauseOfWrite();

        Chunk* firstChunk;
        Chunk* lastChunk;
        uint32 size = src.buffer()->detachData(firstChunk, lastChunk);

        while(size > maxSize)
        {
            uint32 oversize = size - maxSize;

            if(oversize > lastChunk->size())
            {
                size -= lastChunk->size();
                delete std::exchange(lastChunk, lastChunk->prev());

                if(!lastChunk)
                {
                    dbgAssert(!size);
                    dbgAssert(!maxSize);
                    return 0;
                }
            }
            else
            {
                lastChunk->setEnd(static_cast<uint16>(lastChunk->end() - oversize));
                break;
            }
        }

        write(firstChunk, lastChunk, size);

        return size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::write(Cursor& src, uint32 maxSize)
    {
        return writeImpl(performing::PartCursorPromotor(src, maxSize));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Alter::write(Chunk* srcFirst, Chunk* srcLast, uint32 size)
    {
        dbgHeavyAssert(consistent());

        if(!_container)
        {
            dbgWarn("unable to enlarge missing container");
            abort();
        }

        _preparedWriteBuffer.reset();
        copyBufferBecauseOfWrite();

        if(!_container->buffer())
        {
            //нет буфера, принять готовый контент
            _chunkPos = srcLast->size();
            _chunk = srcLast;
            _pos = size;

            _container->setBuffer(new Buffer{srcFirst, srcLast, size});
            dbgHeavyAssert(consistent());
            return;
        }

        if(!_chunk)
        {
            //буфер пустой, принять готовый контент
            dbgAssert(!_container->buffer()->firstChunk());
            dbgAssert(!_container->buffer()->lastChunk());
            dbgAssert(!_container->buffer()->size());

            _chunkPos = srcLast->size();
            _chunk = srcLast;
            _pos = size;

            _container->buffer()->setFirstChunk(srcFirst);
            _container->buffer()->setLastChunk(srcLast);
            _container->buffer()->addSize(size);
            dbgHeavyAssert(consistent());
            return;
        }

        dbgAssert(_chunk);

        if(_chunkPos > 0 && _chunkPos < _chunk->size())
        {
            //порвать текущий
            Chunk* next = new Chunk{_chunk, _chunk->next(), 0, static_cast<uint16>(_chunk->size() - _chunkPos)};

            if(next->next())
            {
                next->next()->setPrev(next);
            }

            _chunk->setNext(next);

            memcpy(next->data(), _chunk->data() + _chunkPos, _chunk->size() - _chunkPos);
            _chunk->setEnd(_chunk->begin() + _chunkPos);
        }

        if(_chunkPos >= _chunk->size())
        {
            //после текущего
            if(_chunk->next())
            {
                //не последний
                _chunk->next()->setPrev(srcLast);
                srcLast->setNext(_chunk->next());

                _chunk->setNext(srcFirst);
                srcFirst->setPrev(_chunk);

                _chunk = srcLast->next();
                _chunkPos = 0;
            }
            else
            {
                //последний
                srcLast->setNext(nullptr);

                _chunk->setNext(srcFirst);
                srcFirst->setPrev(_chunk);

                _chunk = srcLast;
                _chunkPos = _chunk->size();

                _container->buffer()->setLastChunk(_chunk);
            }
        }
        else //if(!_chunkPos)
        {
            //перед текущим
            dbgAssert(!_chunkPos);

            if(_chunk->prev())
            {
                //не первый
                _chunk->prev()->setNext(srcFirst);
                srcFirst->setPrev(_chunk->prev());

                srcLast->setNext(_chunk);
                _chunk->setPrev(srcLast);
            }
            else
            {
                //первый
                _chunk->setPrev(srcLast);
                srcLast->setNext(_chunk);

                _container->buffer()->setFirstChunk(srcFirst);
            }
        }

        _pos += size;
        _container->buffer()->addSize(size);

        dbgHeavyAssert(consistent());
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::remove(uint32 maxSize)
    {
        return removeImpl(maxSize,
            [](void* data, uint32 size)
            {
                (void)data;
                (void)size;
            },
            [](Chunk* first, Chunk* last, uint32 size)
            {
                (void)size;
                for(;;)
                {
                    if(first == last)
                    {
                        delete first;
                        break;
                    }
                    else
                    {
                        delete std::exchange(first, first->next());
                    }
                }
            });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::removeTo(void* dst, uint32 maxSize)
    {
        byte* dstIterator = static_cast<byte*>(dst);
        return removeImpl(maxSize,
            [&](void* data, uint32 size)
            {
                ::memcpy(dstIterator, data, size);
                dstIterator += size;
            },
            [&](Chunk* first, Chunk* last, uint32 size)
            {
                (void)size;
                for(;;)
                {
                    if(first == last)
                    {
                        ::memcpy(dstIterator, first->data(), first->size());
                        dstIterator += first->size();
                        delete first;
                        break;
                    }
                    else
                    {
                        ::memcpy(dstIterator, first->data(), first->size());
                        dstIterator += first->size();
                        delete std::exchange(first, first->next());
                    }
                }
            });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::removeTo(Bytes& dst, uint32 maxSize)
    {
        Alter alter(dst.end());
        return removeTo(alter, maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::removeTo(Alter& dst, uint32 maxSize)
    {
        return removeImpl(maxSize,
            [&](void* data, uint32 size)
            {
                dst.write(data, size);
            },
            [&](Chunk* first, Chunk* last, uint32 size)
            {
                first->setPrev(nullptr);
                last->setNext(nullptr);

                dst.write(first, last, size);
            });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::writeImpl(auto&& src)
    {
        dbgHeavyAssert(consistent());

        _preparedWriteBuffer.reset();
        copyBufferBecauseOfWrite();

        uint32 wrote = 0;

        return performing::execute(
            performing::AlterPromotor(*this),
            src,
            [&](void* ownData, const void* rivalData, uint32 stepSize)
            {
                if(!stepSize)
                {
                    return performing::StepResult{true, wrote};
                }

                std::memcpy(ownData, rivalData, stepSize);
                wrote += stepSize;

                return performing::StepResult{false, wrote};
            });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Alter::removeImpl(uint32 maxSize, auto&& utilizeRaw, auto&& utilizeChunks)
    {
        dbgHeavyAssert(consistent());

        if(!_container)
        {
            dbgWarn("unable to remove, missing container");
            abort();
        }

        _preparedWriteBuffer.reset();
        copyBufferBecauseOfWrite();

        if(!_container->buffer())
        {
            //нет буфера
            return 0;
        }

        if(!_chunk)
        {
            //буфер пустой
            dbgAssert(!_container->buffer()->firstChunk());
            dbgAssert(!_container->buffer()->lastChunk());
            dbgAssert(!_container->buffer()->size());

            return 0;
        }

        dbgAssert(_chunk);

        uint32 size = maxSize = std::min(maxSize, _container->buffer()->size() - _pos);
        _container->buffer()->decSize(size);

        //начало
        if(_chunkPos > 0 && _chunkPos < _chunk->size())
        {
            //в текущем вырезать кусок
            uint32 tail = _chunk->size() - _chunkPos;
            if(tail <= size)
            {
                //до конца
                utilizeRaw(_chunk->data() + _chunkPos, tail);
                _chunk->setEnd(_chunk->begin() + _chunkPos);
                size -= tail;

                if(_chunk->next())
                {
                    _chunk = _chunk->next();
                    _chunkPos = 0;
                }
            }
            else
            {
                //часть
                utilizeRaw(_chunk->data() + _chunkPos, size);
                uint32 tailMove = tail - size;
                ::memmove(
                            _chunk->data()+_chunkPos,
                            _chunk->data()+_chunkPos+size,
                            tailMove);
                size = 0;

                dbgHeavyAssert(consistent());
                return maxSize;
            }
        }

        //серидина, цельные чанки и последний частично
        {
            Chunk* keepFrontLast = _chunk->prev();
            Chunk* keepTailFirst = _chunk;

            //прокручивать чанки
            {
                Chunk* removeFirst = keepTailFirst;
                Chunk* removeLast = nullptr;
                uint32 removeSize = 0;

                for(;;)
                {
                    if(!size)
                    {
                        if(removeLast)
                        {
                            dbgAssert(removeSize);
                            utilizeChunks(removeFirst, removeLast, removeSize);
                        }

                        break;
                    }

                    dbgAssert(keepTailFirst);
                    if(size < keepTailFirst->size())
                    {
                        if(removeLast)
                        {
                            dbgAssert(removeSize);
                            utilizeChunks(removeFirst, removeLast, removeSize);
                        }

                        //конец
                        utilizeRaw(keepTailFirst->data(), size);
                        keepTailFirst->setBegin(static_cast<uint16>(keepTailFirst->begin() + size));
                        break;
                    }

                    removeLast = keepTailFirst;
                    removeSize += keepTailFirst->size();
                    size -= keepTailFirst->size();
                    keepTailFirst = keepTailFirst->next();
                }
            }

            if(keepTailFirst != _chunk)
            {
                if(keepTailFirst)
                {
                    keepTailFirst->setPrev(keepFrontLast);

                    if(keepFrontLast)
                    {
                        keepFrontLast->setNext(keepTailFirst);
                    }
                    else
                    {
                        _container->buffer()->setFirstChunk(keepTailFirst);
                    }

                    _chunk = keepTailFirst;
                    _chunkPos = 0;
                }
                else
                {
                    //срезано до конца
                    if(keepFrontLast)
                    {
                        keepFrontLast->setNext(nullptr);
                        _container->buffer()->setLastChunk(keepFrontLast);
                        _chunk = keepFrontLast;
                        _chunkPos = _chunk->size();
                    }
                    else
                    {
                        //и от начала
                        _container->buffer()->setFirstChunk(nullptr);
                        _container->buffer()->setLastChunk(nullptr);
                        _chunk = nullptr;
                        _chunkPos = 0;
                    }
                }
            }
        }

        dbgHeavyAssert(consistent());
        return maxSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Alter::consistent() const
    {
        if(!Cursor::consistent())
        {
            dbgAssert(false);
            return false;
        }

        if(_preparedWriteBuffer)
        {
            if(_preparedWriteBuffer->size())
            {
                dbgAssert(false);
                return false;
            }

            if(_preparedWriteBuffer->begin() > 0)
            {
                dbgAssert(false);
                return false;
            }

            if(size() > 0)
            {
                dbgAssert(false);
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Alter::copyBufferBecauseOfWrite()
    {
        dbgHeavyAssert(consistent());

        if(!_container)
        {
            return;
        }

        Cursor::resetOtherUsersInContainer();

        if(_container->buffer() && _container->buffer()->shareCounter() > 1)
        {
            {
                Bytes otherContainer;
                Alter otherAlert(otherContainer.begin());

                Chunk* c = _container->buffer()->firstChunk();

                while(c)
                {
                    otherAlert.write(c->data(), c->size());
                    c = c->next();
                }

                _container->resetBuffer();
                _container->setBuffer(otherContainer.buffer());
            }


            uint32 oldPos = _pos;
            _chunk = _container->buffer()->firstChunk();
            _chunkPos = 0;
            _pos = 0;

            if(oldPos)
            {
                Cursor::advance(static_cast<int32>(oldPos));
            }
        }

        dbgHeavyAssert(consistent());
    }
}
