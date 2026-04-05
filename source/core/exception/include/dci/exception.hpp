/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "exception/api.hpp"
#include "exception/skeleton.hpp"
#include "exception/buildInstance.hpp"
#include "exception/toString.hpp"
#include "eid.hpp"
#include <exception>
#include <string>

namespace dci
{
    class API_DCI_EXCEPTION Exception
        : public std::exception
    {
    public:
        Exception() = default;
        Exception(std::string_view what);
        Exception(const std::string& what);
        Exception(std::string&& what);

        template <auto size>
        Exception(const char(&what)[size])
            : Exception{std::string_view{what, size-1}}
        {
        }

        virtual const std::string_view name() const;
        virtual const Eid& eid() const;// uuidgen | sed -r 's/(..)-?/0x\1,/g' | sed -e 's/^/static constexpr Eid _eid {/' -e 's/,$/}/'

    public:
        const char* what() const noexcept override;
        const std::string& whatBuffer() const noexcept;
        std::string& whatBuffer() noexcept;

    public:
        // uuidgen | sed -r 's/(..)-?/0x\1,/g' | sed -e 's/^/static constexpr Eid _eid {/' -e 's/,$/}/'
        static constexpr Eid _eid {0xfa,0x4a,0x4e,0xf8,0x0d,0x07,0x46,0xf5,0xae,0x92,0x14,0x57,0x7e,0x86,0x1d,0xef};

    protected:
        std::string _what;
    };
}
