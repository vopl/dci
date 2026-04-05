/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "container.hpp"

namespace dci::bytes
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Container::Container()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Container::Container(const Container& from)
        : _buffer(from._buffer)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Container::Container(Container&& from)
        : _buffer(static_cast<BufferPtr&&>(from._buffer))
    {
        while(from._firstUser)
        {
            User* u = from._firstUser;
            dbgAssert(&from == u->_container);
            u->reassignContainer(this);
            dbgAssert(this == u->_container);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Container::~Container()
    {
        while(_firstUser)
        {
            dbgAssert(this == _firstUser->_container);
            _firstUser->reset();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Container& Container::operator=(const Container& from)
    {
        if(_buffer != from._buffer)
        {
            _buffer = from._buffer;
            while(_firstUser)
            {
                dbgAssert(this == _firstUser->_container);
                _firstUser->reset();
            }
        }
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Container& Container::operator=(Container&& from)
    {
        if(_buffer != from._buffer)
        {
            _buffer = std::move(from._buffer);
            while(from._firstUser)
            {
                User* u = from._firstUser;
                dbgAssert(&from == u->_container);
                u->reassignContainer(this);
                dbgAssert(this == u->_container);
            }
        }
        else
        {
            from._buffer.reset();
            while(from._firstUser)
            {
                dbgAssert(&from == _firstUser->_container);
                from._firstUser->reset();
            }
        }
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Container::resetUsers()
    {
        while(_firstUser)
        {
            dbgAssert(this == _firstUser->_container);
            _firstUser->reset();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Buffer* Container::buffer()
    {
        return _buffer.get();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const bytes::Buffer* Container::buffer() const
    {
        return _buffer.get();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Container::setBuffer(bytes::Buffer* buffer)
    {
        dbgAssert(!_buffer);
        _buffer.reset(buffer);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Container::resetBuffer()
    {
        _buffer.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Container::track(User* user)
    {
        dbgAssert(this == user->_container);
        dbgAssert(!user->_prev);
        dbgAssert(!user->_next);

        if(_firstUser)
        {
            dbgAssert(!_firstUser->_prev);
            user->_next = _firstUser;
            _firstUser->_prev = user;
            _firstUser = user;
        }
        else
        {
            _firstUser = user;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Container::untrack(User* user)
    {
        dbgAssert(this == user->_container || !user->_container);

        if(_firstUser == user)
        {
            dbgAssert(!user->_prev);
            _firstUser = user->_next;
            user->_next = nullptr;
            if(_firstUser)
            {
                _firstUser->_prev = nullptr;
            }
        }
        else
        {
            if(user->_next)
            {
                user->_next->_prev = user->_prev;
            }

            if(user->_prev)
            {
                user->_prev->_next = user->_next;
            }

            user->_next = nullptr;
            user->_prev = nullptr;
        }
    }
}
