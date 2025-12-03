// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstddef>

namespace dci::mm::impl::vm
{
    using TVmAccessHandler = bool (*)(void* addr);
    using TVmPanic = void (*)(int signum);

    bool init(TVmAccessHandler accessHandler, TVmPanic panic);
    bool deinit(TVmAccessHandler accessHandler);

    void* alloc(std::size_t size);
    bool free(void* addr, std::size_t size);

    enum class Protection
    {
        none,
#ifdef _WIN32
        guard,
#endif
        rw,
    };

    bool protect(void* addr, std::size_t size, Protection protection);
}
