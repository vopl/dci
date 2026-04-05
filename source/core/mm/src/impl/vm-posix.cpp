/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "vm.hpp"

#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>

#include <iostream>

/*
 * системное ограничение на количество сегментов vm для процесса,
 * /proc/sys/vm/max_map_count,
 * sysctl vm.max_map_count=16777216
 *
 * по этому лимиту будет отваливаться mprotect с ошибкой ENOMEM
 *
 *
*/

namespace dci::mm::impl::vm
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        extern char g_stateArea[];

        struct State
        {
            TVmAccessHandler        _accessHandler {};
            TVmPanic                _panic {};

            static constexpr size_t _altStackSize {16 * 1024 * 1024};
            char                    _altStackArea[_altStackSize] {};
            ::stack_t               _oldAltStack {};
            struct ::sigaction      _oldAction {};

            void * operator new(size_t size)
            {
                (void)size;
                return &g_stateArea;
            }

            void operator delete(void* ptr)
            {
                (void)ptr;
            }
        };
        char g_stateArea[sizeof(State)];

        State* g_state = nullptr;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void segvHandler(int signal_number, siginfo_t* info, void* ctx)
        {
            State* state = g_state;

            if(state)
            {
                if(state->_accessHandler(info->si_addr))
                {
                    return;
                }

                char buf[64];
                std::sprintf(buf, "%p", info->si_addr);
                fputs("unhandled SIGSEGV for ", stderr);
                fputs(buf, stderr);
                fputs(", do panic\n", stderr);
                fflush(stderr);
                if(state->_panic)
                {
                    state->_panic(signal_number);
                }

                std::sprintf(buf, "%p", info->si_addr);
                fputs("call SIGSEGV default handler for ", stderr);
                fputs(buf, stderr);
                fputs("\n", stderr);
                fflush(stderr);

                if(state->_oldAction.sa_flags & SA_SIGINFO)
                {
                    return state->_oldAction.sa_sigaction(signal_number, info, ctx);
                }
                else
                {
                    return state->_oldAction.sa_handler(signal_number);
                }
            }

            char buf[64];
            std::sprintf(buf, "%p", info->si_addr);
            fputs("unable to handle SIGSEGV for ", stderr);
            fputs(buf, stderr);
            fputs("\n", stderr);
            fflush(stderr);
            std::abort();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool init(TVmAccessHandler accessHandler, TVmPanic panic)
    {
        if(g_state)
        {
            std::fprintf(stderr, "vm::init: secondary call\n");
            std::fflush(stderr);
            return false;
        }
        g_state = new State;
        g_state->_accessHandler = accessHandler;
        g_state->_panic = panic;

        ::stack_t altstack;
        memset(&altstack, 0, sizeof(altstack));
        altstack.ss_size = g_state->_altStackSize;
        altstack.ss_sp = g_state->_altStackArea;
        altstack.ss_flags = 0;
        if(sigaltstack(&altstack, &g_state->_oldAltStack))
        {
            delete g_state;
            g_state = nullptr;
            perror("sigaltstack");
            return false;
        }

        struct ::sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = &segvHandler;
        sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
        sigfillset (&sa.sa_mask);
        if(sigaction(SIGSEGV, &sa, &g_state->_oldAction))
        {
            delete g_state;
            g_state = nullptr;
            perror("sigaction");
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool deinit(TVmAccessHandler accessHandler)
    {
        if(!g_state)
        {
            std::fprintf(stderr, "vm::deinit: already deinited\n");
            std::fflush(stderr);
            return false;
        }

        if(g_state->_accessHandler != accessHandler)
        {
            std::fprintf(stderr, "vm::deinit: wrong accessHandler\n");
            std::fflush(stderr);
            return false;
        }

        if(sigaction(SIGSEGV, &g_state->_oldAction, nullptr))
        {
            perror("sigaction");
        }

        if(sigaltstack(&g_state->_oldAltStack, nullptr))
        {
            perror("sigaltstack");
        }

        delete g_state;
        g_state = nullptr;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void* alloc(std::size_t size)
    {
        void* addr = mmap(
                            nullptr,
                            size,
                            PROT_NONE,
                            MAP_ANONYMOUS|MAP_PRIVATE,
                            0,
                            0);

        if(MAP_FAILED == addr)
        {
            perror("mmap");
            return nullptr;
        }


        if(madvise(addr, size, MADV_DONTDUMP))
        {
            perror("madvise");
            munmap(addr, size);//ignore error
            return nullptr;
        }

        return addr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool free(void* addr, std::size_t size)
    {
        if(munmap(addr, size))
        {
            perror("munmap");
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool protect(void* addr, std::size_t size, Protection protection)
    {
        switch(protection)
        {
        case Protection::none:
            if(mprotect(addr, size, PROT_NONE))
            {
                perror("mprotect");
                return false;
            }

            if(madvise(addr, size, MADV_DONTDUMP))
            {
                perror("madvise");
                return false;
            }
            break;

        case Protection::rw:
            if(mprotect(addr, size, PROT_READ|PROT_WRITE))
            {
                perror("mprotect");
                return false;
            }

            if(madvise(addr, size, MADV_DODUMP))
            {
                perror("madvise");
                return false;
            }
            break;
        }

        return true;
    }
}
