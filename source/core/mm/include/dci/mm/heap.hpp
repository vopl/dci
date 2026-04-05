/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once
#include <cstddef>
#include "api.hpp"

#include <dci/himpl.hpp>

namespace dci::mm::heap
{
    API_DCI_MM void* alloc(std::size_t size);
    API_DCI_MM void free(void* ptr);

    template <std::size_t size> void* alloc();
    template <std::size_t size> void free(void* ptr);


    ////////////////////////////////////////////////////////////////
    static constexpr std::size_t _sizeClassMin = 8;
    static constexpr std::size_t _sizeClassMax = 4096;
    static constexpr std::size_t _sizeClassStep = 16;

    ////////////////////////////////////////////////////////////////
    namespace details
    {
        template <std::size_t sizeClass> API_DCI_MM void* allocBySizeClass();
        template <std::size_t sizeClass> API_DCI_MM void freeBySizeClass(void* ptr);

        inline constexpr std::size_t evalSizeClass(std::size_t size)
        {
            return size <= _sizeClassMin ? _sizeClassMin :
                   size > _sizeClassMax ? _sizeClassMax :
                   size/_sizeClassStep*_sizeClassStep == size ? size :
                   size/_sizeClassStep*_sizeClassStep + _sizeClassStep;
        }
    }

    ////////////////////////////////////////////////////////////////
    template <std::size_t size> void* alloc()
    {
        if(size > _sizeClassMax)
        {
            return alloc(size);
        }
        return details::allocBySizeClass<details::evalSizeClass(size)>();
    }

    ////////////////////////////////////////////////////////////////
    template <std::size_t size> void free(void* ptr)
    {
        if(size > _sizeClassMax)
        {
            return free(ptr);
        }
        return details::freeBySizeClass<details::evalSizeClass(size)>(ptr);
    }
}

