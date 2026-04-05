/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "cursor.hpp"
#include "alter.hpp"
#include "../../impl/bytes.hpp"
#include "../performing/alterPromotor.hpp"
#include "../performing/cursorPromotor.hpp"
#include "../performing/partCursorPromotor.hpp"
#include "../performing/arrayPromotor.hpp"
#include "../performing/stepResult.hpp"
#include "../performing/execute.hpp"

namespace dci::bytes::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cursor::tryDestruction(Cursor* c)
    {
        c->User::reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor()
    {
        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(const Cursor& from)
        : User(from)
    {
        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(Cursor&& from)
        : User(static_cast<User&&>(from))
    {
        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(Bytes* container, bool posAtBegin)
        : User(container, posAtBegin)
    {
        dbgHeavyAssert(consistent());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::~Cursor()
    {
        dbgHeavyAssert(consistent());
        tryDestruction(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor& Cursor::operator=(const Cursor& from)
    {
        dbgHeavyAssert(consistent());

        User::operator=(from);

        dbgHeavyAssert(consistent());

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor& Cursor::operator=(Cursor&& from)
    {
        dbgHeavyAssert(consistent());

        User::operator=(static_cast<User&&>(from));

        dbgHeavyAssert(consistent());

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Cursor::atBegin() const
    {
        return !_pos;
    }

    bool Cursor::atEnd() const
    {
        dbgHeavyAssert(consistent());

        if(!_chunk)
        {
            return true;
        }

        return !_chunk->next() && _chunkPos >= _chunk->size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::pos() const
    {
        dbgHeavyAssert(consistent());
        return _pos;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::size() const
    {
        dbgHeavyAssert(consistent());

        if(_container && _container->buffer())
        {
            return _container->buffer()->size() - _pos;
        }

        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::sizeBack() const
    {
        dbgHeavyAssert(consistent());
        return _pos;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int32 Cursor::advance(int32 offset)
    {
        dbgHeavyAssert(consistent());

        int32 advanced = 0;

        if(_chunk)
        {
            if(offset > 0)
            {
                //fwd
                for(;;)
                {
                    int32 can = _chunk->size() - _chunkPos;

                    if(can > offset)
                    {
                        _chunkPos += static_cast<uint16>(offset);
                        _pos += static_cast<uint32>(offset);
                        advanced += offset;
                        offset = 0;
                        break;
                    }
                    else
                    {
                        offset -= can;
                        advanced += can;
                        _pos += static_cast<uint32>(can);

                        if(_chunk->next())
                        {
                            _chunk = _chunk->next();
                            _chunkPos = 0;
                        }
                        else
                        {
                            _chunkPos += can;
                            break;
                        }
                    }
                }
            }
            else if(offset < 0)
            {
                //bwd
                offset = -offset;

                for(;;)
                {
                    int32 can = _chunkPos;

                    if(can >= offset)
                    {
                        _chunkPos -= offset;
                        _pos -= static_cast<uint32>(offset);
                        advanced += offset;
                        offset = 0;
                        break;
                    }
                    else
                    {
                        if(can)
                        {
                            offset -= can;
                            advanced += can;
                            _pos -= static_cast<uint32>(can);
                            _chunkPos = 0;
                        }
                        else
                        {
                            if(_chunk->prev())
                            {
                                _chunk = _chunk->prev();
                                _chunkPos = _chunk->size();
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }

                advanced = -advanced;
            }
        }

        dbgHeavyAssert(consistent());

        return advanced;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int32 Cursor::advanceChunks(int32 amount)
    {
        dbgHeavyAssert(consistent());

        int32 advanced = 0;

        if(_chunk)
        {
            _pos -= _chunkPos;
            _chunkPos = 0;

            if(amount > 0)
            {
                //fwd
                while(amount)
                {
                    if(_chunk->next())
                    {
                        _pos += _chunk->size();
                        _chunk = _chunk->next();
                        amount--;
                        advanced++;
                    }
                    else
                    {
                        _pos += _chunk->size();
                        _chunkPos = _chunk->size();
                        amount--;
                        advanced++;
                        break;
                    }
                }
            }
            else if(amount < 0)
            {
                //bwd
                while(amount && _chunk->prev())
                {
                    _chunk = _chunk->prev();
                    _pos -= _chunk->size();
                    amount++;
                    advanced--;
                }
            }
        }

        dbgHeavyAssert(consistent());

        return advanced;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const byte* Cursor::continuousData() const
    {
        dbgHeavyAssert(consistent());

        if(_chunk && _chunk->size() > _chunkPos)
        {
            return _chunk->data() + _chunkPos;
        }

        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::continuousDataSize() const
    {
        dbgHeavyAssert(consistent());

        if(_chunk)
        {
            return _chunk->size() - _chunkPos;
        }

        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::continuousDataOffset() const
    {
        if(_chunk)
        {
            return _chunk->begin() + _chunkPos;
        }

        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::read(void* dst, uint32 maxSize)
    {
        dbgHeavyAssert(consistent());
        return readImpl(performing::ArrayPromotor(dst, maxSize), maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::read(Bytes& dst, uint32 maxSize)
    {
        dbgHeavyAssert(consistent());
        Alter a(dst.end());
        return read(a, maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::read(Alter& dst, uint32 maxSize)
    {
        dbgHeavyAssert(consistent());
        return readImpl(performing::AlterPromotor(dst), maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compare(const void* with, uint32 size)
    {
        dbgHeavyAssert(consistent());
        return compareImpl(performing::ArrayPromotor(with, size));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compare(const Bytes& with)
    {
        dbgHeavyAssert(consistent());
        Cursor cursor(with.cbegin());
        return compareImpl(performing::CursorPromotor(cursor));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compare(Cursor& with)
    {
        dbgHeavyAssert(consistent());
        return compareImpl(performing::CursorPromotor(with));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Cursor::toString(uint32 maxSize)
    {
        dbgHeavyAssert(consistent());

        String res;
        res.resize(std::min(size(), maxSize));
        read(res.data(), static_cast<uint32>(res.size()));

        dbgHeavyAssert(consistent());
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Cursor::toHex(uint32 maxSize)
    {
        dbgHeavyAssert(consistent());

        String res;
        res.reserve(std::min(size(), maxSize)*2);

        performing::execute(
            performing::CursorPromotor(*this),
            [&](const void* ownData, uint32 size)
            {
                if(!size)
                {
                    return performing::StepResult{true, 0};
                }

                dbgAssert(ownData);

                size_t curPos = res.size();
                res.resize(curPos+size*2);
                dci::utils::b2h(ownData, size, res.data()+curPos);

                return performing::StepResult{false, 0};
            });

        dbgHeavyAssert(consistent());
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::readImpl(auto&& with, uint32 maxSize)
    {
        dbgHeavyAssert(consistent());

        uint32 readed = 0;

        return performing::execute(
            performing::PartCursorPromotor(*this, maxSize),
            std::forward<decltype(with)>(with),
            [&](const void* ownData, void* rivalData, uint32 size)
            {
                if(!size)
                {
                    return performing::StepResult{true, readed};
                }

                dbgAssert(ownData && rivalData);

                //оба потока продолжаются, копировать контент
                std::memcpy(rivalData, ownData, size);
                readed += size;

                return performing::StepResult{false, readed};
            });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compareImpl(auto&& with)
    {
        dbgHeavyAssert(consistent());
        return performing::execute(
            performing::CursorPromotor(*this),
            std::forward<decltype(with)>(with),
            [](const void* ownData, const void* rivalData, uint32 size)
            {
                if(!size)
                {
                    if(!ownData && !rivalData)
                    {
                        return performing::StepResult{true, std::strong_ordering::equal};
                    }
                    else if(!ownData)
                    {
                        return performing::StepResult{true, std::strong_ordering::less};
                    }
                    else //if(!rivalData)
                    {
                        return performing::StepResult{true, std::strong_ordering::greater};
                    }
                }

                dbgAssert(ownData && rivalData);

                //оба потока продолжаются, сравнивать контент
                int cmp = std::memcmp(ownData, rivalData, size);
                return performing::StepResult{0 != cmp,
                    cmp == 0 ?
                        std::strong_ordering::equal :
                        cmp < 0 ?
                            std::strong_ordering::less :
                            std::strong_ordering::greater};
            });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Cursor::consistent() const
    {
        return User::consistent();
    }

}
