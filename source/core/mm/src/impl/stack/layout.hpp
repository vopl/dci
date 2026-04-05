/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "config.hpp"

#include "header.hpp"
#include "../vm.hpp"

#include <dci/utils/dbg.hpp>
#include <type_traits>
#include <algorithm>
#include <cstdint>
#include <new>
#include <cstdio>

#if __has_include(<alloca.h>)
#   include <alloca.h>
#else
#   include <malloc.h>
#endif

namespace dci::mm::impl::stack
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool stackGrowsDown, bool stackUseGuardPage, bool stackReserveGuardPage = false>
    class Layout;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool stackReserveGuardPage>
    class Layout<false, false, stackReserveGuardPage>
    {
        template <bool, bool, bool> friend class Layout;

    public:
        static constexpr bool _growsDown = false;
        static constexpr bool _hasGuard = false;

    public:
        Layout()
        {
            char* area = reinterpret_cast<char *>(this);
            char* mappedEnd = extend(area, area + std::min(sizeof(Layout), sizeof(_headerArea) + Config::_stackKeepProtectedBytes));

            new (&header()) Header;

            header()._userspaceBegin = area + offsetof(Layout, _userArea);
            header()._userspaceMapped = mappedEnd;
            header()._userspaceEnd = area + offsetof(Layout, _userArea) + sizeof(UserArea);
        }

        ~Layout()
        {
            char* area = reinterpret_cast<char *>(this);
            char* mappedEnd = header()._userspaceMapped;

            header().~Header();

            reduce(mappedEnd, area);
        }

        void compact()
        {
            char* bound = header()._userspaceMapped;
#ifdef _WIN32
            char* onStackPointer = static_cast<char*>(_malloca(1));
#else
            char* onStackPointer = static_cast<char*>(alloca(1));
#endif
            bound = reduce(bound, std::min(reinterpret_cast<char*>(this), onStackPointer + Config::_stackKeepProtectedBytes));
            if(bound != header()._userspaceMapped)
            {
                header()._userspaceMapped = bound;
            }
        }

        bool vmAccessHandler(std::uintptr_t offset)
        {
            char* bound = header()._userspaceMapped;
            bound = extend(bound, reinterpret_cast<char *>(this) + offset);
            if(bound != header()._userspaceMapped)
            {
                header()._userspaceMapped = bound;
            }

            return true;
        }

    private:
        char* reduce(char* oldBound, char* newBound)
        {
            dbgAssert(oldBound >= reinterpret_cast<char *>(this) && oldBound <= reinterpret_cast<char *>(this) + sizeof(*this));
            dbgAssert(newBound >= reinterpret_cast<char *>(this) && newBound <= reinterpret_cast<char *>(this) + sizeof(*this));

            std::uintptr_t inewBound = reinterpret_cast<std::uintptr_t>(newBound);

            if(inewBound % Config::_pageSize)
            {
                inewBound = (inewBound / Config::_pageSize + 1) * Config::_pageSize;
            }

            newBound = reinterpret_cast<char *>(inewBound);

            if(newBound >= oldBound)
            {
                return oldBound;
            }

#ifdef _WIN32
            char* limit = static_cast<char*>(static_cast<void*>(&_userArea)) + sizeof(_userArea);
            if(oldBound + Config::_pageSize <= limit)
            {
                if(!vm::protect(
                            newBound + Config::_pageSize,
                            static_cast<std::size_t>(oldBound - newBound),
                            vm::Protection::none))
                {
                    dbgWarn("unable to protect region");
                    std::abort();
                }
            }
            else
            {
                if(!vm::protect(
                            newBound + Config::_pageSize,
                            static_cast<std::size_t>(oldBound - newBound - Config::_pageSize),
                            vm::Protection::none))
                {
                    dbgWarn("unable to protect region");
                    std::abort();
                }
            }

            if(!vm::protect(
                        newBound,
                        Config::_pageSize,
                        vm::Protection::guard))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }

#else
            if(!vm::protect(
                        newBound,
                        static_cast<std::size_t>(oldBound - newBound),
                        vm::Protection::none))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }
#endif

            return newBound;
        }

        char* extend(char* oldBound, char* newBound)
        {
            dbgAssert(oldBound >= reinterpret_cast<char *>(this));
            dbgAssert(oldBound <= reinterpret_cast<char *>(this) + sizeof(*this));
            dbgAssert(newBound >= reinterpret_cast<char *>(this));
            dbgAssert(newBound <= reinterpret_cast<char *>(this) + sizeof(*this));

            std::uintptr_t inewBound = reinterpret_cast<std::uintptr_t>(newBound);

            if(inewBound % Config::_pageSize)
            {
                inewBound = (inewBound / Config::_pageSize + 1) * Config::_pageSize;
            }

            newBound = reinterpret_cast<char *>(inewBound);

            if(newBound <= oldBound)
            {
                return oldBound;
            }

#ifdef _WIN32
            char* limit = static_cast<char*>(static_cast<void*>(&_userArea)) + sizeof(_userArea);
            if(newBound + Config::_pageSize <= limit)
            {
                if(!vm::protect(
                            newBound,
                            Config::_pageSize,
                            vm::Protection::guard))
                {
                    dbgWarn("unable to protect region");
                    std::abort();
                }
            }
#endif

            if(!vm::protect(
                        oldBound,
                        static_cast<std::size_t>(newBound - oldBound),
                        vm::Protection::rw))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }

            return newBound;
        }

    protected:
        Header& header()
        {
            return _headerArea._header;
        }

    private:
        static constexpr std::size_t _headerAreaSize = sizeof(Header);
        union HeaderArea
        {
            alignas(Header) std::byte _space[_headerAreaSize];
            Header _header;
        };
        static_assert(_headerAreaSize == sizeof(HeaderArea));

        static constexpr std::size_t _userAreaSize = Config::_stackPages * Config::_pageSize - _headerAreaSize - (stackReserveGuardPage ? Config::_pageSize : 0);
        struct UserArea {char _space[_userAreaSize];};

        HeaderArea  _headerArea;
        UserArea    _userArea;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    class Layout<false, true, false>
    {
    public:
        static constexpr bool _growsDown = false;
        static constexpr bool _hasGuard = true;

    public:
        Layout()
        {
            (void)_guardArea;
        }

        ~Layout()
        {
        }

        void compact()
        {
            return _withoutGuard.compact();
        }

        bool vmAccessHandler(std::uintptr_t offset)
        {
            if(offset >= offsetof(Layout, _guardArea))
            {
                fputs("prevent access to stack guard page\n", stderr);
                fflush(stderr);
                return false;
            }

            return _withoutGuard.vmAccessHandler(offset - offsetof(Layout, _withoutGuard));
        }

    protected:
        Header& header()
        {
            return _withoutGuard.header();
        }

    private:
        struct alignas(Config::_pageSize) GuardArea {char _space[Config::_pageSize];};
        using WithoutGuard = Layout<false, false, true>;

        WithoutGuard    _withoutGuard;
        GuardArea       _guardArea;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool stackReserveGuardPage>
    class Layout<true, false, stackReserveGuardPage>
    {
        template <bool, bool, bool> friend class Layout;

    public:
        static constexpr bool _growsDown = true;
        static constexpr bool _hasGuard = false;

    public:
        Layout()
        {
            char* area = reinterpret_cast<char *>(this);
            char* mappedEnd = extend(area + sizeof(Layout), area + sizeof(Layout) - std::min(sizeof(Layout), sizeof(_headerArea) + Config::_stackKeepProtectedBytes));

            new (&header()) Header;

            header()._userspaceBegin = area + offsetof(Layout, _userArea);
            header()._userspaceMapped = mappedEnd;
            header()._userspaceEnd = area + offsetof(Layout, _userArea) + sizeof(UserArea);
        }

        ~Layout()
        {
            char* area = reinterpret_cast<char *>(this);
            char* mappedEnd = header()._userspaceMapped;

            header().~Header();

            reduce(mappedEnd, area + sizeof(Layout));
        }

        void compact()
        {
            char* bound = header()._userspaceMapped;
            bound = reduce(bound, std::max(reinterpret_cast<char*>(this), static_cast<char*>(alloca(1)) - Config::_stackKeepProtectedBytes));
            if(bound != header()._userspaceMapped)
            {
                header()._userspaceMapped = bound;
            }
        }

        bool vmAccessHandler(std::uintptr_t offset)
        {
            char* bound = header()._userspaceMapped;
            bound = extend(bound, reinterpret_cast<char *>(this) + offset);
            if(bound != header()._userspaceMapped)
            {
                header()._userspaceMapped = bound;
            }

            return true;
        }

    private:
        char* reduce(char* oldBound, char* newBound)
        {
            dbgAssert(oldBound >= reinterpret_cast<char *>(this));
            dbgAssert(oldBound <= reinterpret_cast<char *>(this) + sizeof(*this));
            dbgAssert(newBound >= reinterpret_cast<char *>(this));
            dbgAssert(newBound <= reinterpret_cast<char *>(this) + sizeof(*this));

            std::uintptr_t inewBound = reinterpret_cast<std::uintptr_t>(newBound);

            if(inewBound % Config::_pageSize)
            {
                inewBound = inewBound - inewBound % Config::_pageSize;
            }

            newBound = reinterpret_cast<char *>(inewBound);

            if(newBound <= oldBound)
            {
                return oldBound;
            }

#ifdef _WIN32
            char* limit = static_cast<char*>(static_cast<void*>(&_userArea));
            if(oldBound - Config::_pageSize >= limit)
            {
                if(!vm::protect(
                            oldBound-Config::_pageSize,
                            static_cast<std::size_t>(newBound - oldBound),
                            vm::Protection::none))
                {
                    dbgWarn("unable to protect region");
                    std::abort();
                }
            }
            else
            {
                if(!vm::protect(
                            oldBound,
                            static_cast<std::size_t>(newBound - oldBound - Config::_pageSize),
                            vm::Protection::none))
                {
                    dbgWarn("unable to protect region");
                    std::abort();
                }
            }

            if(!vm::protect(
                        newBound-Config::_pageSize,
                        Config::_pageSize,
                        vm::Protection::guard))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }
#else
            if(!vm::protect(
                        oldBound,
                        static_cast<std::size_t>(newBound - oldBound),
                        vm::Protection::none))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }
#endif

            return newBound;
        }

        char* extend(char* oldBound, char* newBound)
        {
            dbgAssert(oldBound >= reinterpret_cast<char *>(this));
            dbgAssert(oldBound <= reinterpret_cast<char *>(this) + sizeof(*this));
            dbgAssert(newBound >= reinterpret_cast<char *>(this));
            dbgAssert(newBound <= reinterpret_cast<char *>(this) + sizeof(*this));

            std::uintptr_t inewBound = reinterpret_cast<std::uintptr_t>(newBound);

            if(inewBound % Config::_pageSize)
            {
                inewBound = inewBound - inewBound % Config::_pageSize;
            }

            newBound = reinterpret_cast<char *>(inewBound);

            if(newBound >= oldBound)
            {
                return oldBound;
            }

#ifdef _WIN32
            char* limit = static_cast<char*>(static_cast<void*>(&_userArea));
            if(newBound - Config::_pageSize >= limit)
            {
                if(!vm::protect(
                            newBound - Config::_pageSize,
                            Config::_pageSize,
                            vm::Protection::guard))
                {
                    dbgWarn("unable to protect region");
                    std::abort();
                }
            }
#endif

            if(!vm::protect(
                        newBound,
                        static_cast<std::size_t>(oldBound - newBound),
                        vm::Protection::rw))
            {
                dbgWarn("unable to protect region");
                std::abort();
            }

            return newBound;
        }

    protected:
        Header& header()
        {
            return _headerArea._header;
        }

    private:
        static constexpr std::size_t _headerAreaSize = sizeof(Header);
        union HeaderArea
        {
            alignas(Header) std::byte _space[_headerAreaSize];
            Header _header;
        };
        static_assert(_headerAreaSize == sizeof(HeaderArea));

        static constexpr std::size_t _userAreaSize = Config::_stackPages * Config::_pageSize - _headerAreaSize - (stackReserveGuardPage ? Config::_pageSize : 0);
        struct UserArea {char _space[_userAreaSize];};

        UserArea    _userArea;
        HeaderArea  _headerArea;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    class Layout<true, true, false>
    {
    public:
        static constexpr bool _growsDown = true;
        static constexpr bool _hasGuard = true;

    public:
        Layout()
        {
            (void)_guardArea;
        }

        ~Layout()
        {
        }

        void compact()
        {
            return _withoutGuard.compact();
        }

        bool vmAccessHandler(std::uintptr_t offset)
        {
            if(offset <= offsetof(Layout, _withoutGuard))
            {
                fputs("prevent access to stack guard page\n", stderr);
                fflush(stderr);
                return false;
            }

            return _withoutGuard.vmAccessHandler(offset - offsetof(Layout, _withoutGuard));
        }

    protected:
        Header& header()
        {
            return _withoutGuard.header();
        }

    private:
        struct alignas(Config::_pageSize) GuardArea {char _space[Config::_pageSize];};
        using WithoutGuard = Layout<true, false, true>;

        GuardArea       _guardArea;
        WithoutGuard    _withoutGuard;
    };
}
