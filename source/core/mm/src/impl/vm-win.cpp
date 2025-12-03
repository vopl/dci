// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "vm.hpp"
#include <dci/utils/dbg.hpp>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <windows.h>

namespace dci::mm::impl::vm
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        extern char g_stateArea[];

        struct State
        {
            TVmAccessHandler    _accessHandler {};
            TVmPanic            _panic {};
            PVOID               _VEH;

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
        LONG NTAPI vectoredExceptionHandler(struct _EXCEPTION_POINTERS *info)
        {
            if(info->ExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
            {
                return EXCEPTION_CONTINUE_SEARCH;
            }

            if(EXCEPTION_ACCESS_VIOLATION != info->ExceptionRecord->ExceptionCode)
            {
                return EXCEPTION_CONTINUE_SEARCH;
            }

            void* addr = info->ExceptionRecord->ExceptionAddress;
            State* state = g_state;

            if(state)
            {
                if(state->_accessHandler(addr))
                {
                    return EXCEPTION_CONTINUE_EXECUTION;
                }

                std::fprintf(stderr, "unhandled AV for 0x%p, do panic\n", addr);
                std::fflush(stderr);
                if(state->_panic)
                {
                    state->_panic(SIGSEGV);
                }
            }
            else
            {
                std::fprintf(stderr, "unable to handle AV for 0x%p\n", addr);
                std::fflush(stderr);
            }

            return EXCEPTION_CONTINUE_SEARCH;
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

        g_state->_VEH = AddVectoredExceptionHandler(1, &vectoredExceptionHandler);

        return !!g_state->_VEH;
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

        if(g_state->_VEH)
        {
            ULONG res = RemoveVectoredExceptionHandler(g_state->_VEH);
            (void)res;
            g_state->_VEH = nullptr;
        }

        delete g_state;
        g_state = nullptr;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void* alloc(std::size_t size)
    {
        void* addr = VirtualAlloc(
                            nullptr,
                            size,
                            MEM_RESERVE,
                            PAGE_NOACCESS);

        if(!addr)
        {
            std::fprintf(stderr, "vm::alloc: VirtualAlloc failed: %lu\n", GetLastError());
            std::fflush(stderr);
            return nullptr;
        }

        return addr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool free(void* addr, std::size_t size)
    {
#ifndef MEM_COALESCE_PLACEHOLDERS
#   define MEM_COALESCE_PLACEHOLDERS 0x00000001
#endif
        if(!VirtualFree(addr, size, MEM_RELEASE | MEM_COALESCE_PLACEHOLDERS))
        {
            std::fprintf(stderr, "vm::free: VirtualFree failed: %lu\n", GetLastError());
            std::fflush(stderr);
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
            if(!VirtualFree(addr, size, MEM_DECOMMIT))
            {
                std::fprintf(stderr, "vm::protect: VirtualFree failed: %lu\n", GetLastError());
                std::fflush(stderr);
                return false;
            }
            break;

        case Protection::guard:
        case Protection::rw:
            if(addr != VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE | (Protection::guard == protection ? PAGE_GUARD : 0)))
            {
                std::fprintf(stderr, "vm::protect: VirtualAlloc failed: %lu\n", GetLastError());
                std::fflush(stderr);
                return false;
            }
        }

        return true;
    }
}
