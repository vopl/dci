// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#define likely(x) (x)
#define unlikely(x) (x)

//TODO: use std::unreachable instead
#define unreacheable() dci::utils::unreacheableImpl()

namespace dci::utils
{
    [[noreturn]] inline void unreacheableImpl()
    {
        // Uses compiler specific extensions if possible.
        // Even if no extension is used, undefined behavior is still raised by
        // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
        __assume(false);
#else // GCC, Clang
        __builtin_unreachable();
#endif
    }
}

