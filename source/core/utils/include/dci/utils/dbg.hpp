/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#ifdef NDEBUG
#   define dbgAssert(condition) ((void)0)
#   define dbgHeavyAssert(condition) ((void)0)
#   define dbgAssertX(condition, msg) ((void)0)
#   define dbgWarn(msg) ((void)0)
#   define dbgFatal(msg) ((void)0)
#else

#   define dbgAssert(condition)                                                 \
     (static_cast<bool>(condition)                                              \
      ? ((void)0)                                                               \
      : dci::utils::dbg::assertWarn(#condition, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#   define dbgHeavyAssert(condition) ((void)0)

#   define dbgAssertX(condition, msg)                                                                       \
     (static_cast<bool>(condition)                                                                          \
      ? ((void)0)                                                                                           \
      : dci::utils::dbg::assertWarn(#msg " on condition: [" #condition "]" , __FILE__, __LINE__, __PRETTY_FUNCTION__))

#   define dbgWarn(msg) dci::utils::dbg::warn(msg, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#   define dbgFatal(msg) dci::utils::dbg::fatal(msg, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#endif

#include "api.hpp"

namespace dci::utils::dbg
{
                 API_DCI_UTILS void warn (const char* msg, const char* file, unsigned int line, const char* function);
    [[noreturn]] API_DCI_UTILS void fatal(const char* msg, const char* file, unsigned int line, const char* function);

                 API_DCI_UTILS void assertWarn (const char* assertion, const char* file, unsigned int line, const char* function);
    [[noreturn]] API_DCI_UTILS void assertFatal(const char* assertion, const char* file, unsigned int line, const char* function);
}
