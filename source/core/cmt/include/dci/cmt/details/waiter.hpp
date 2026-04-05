/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/api.hpp>
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>
#include <dci/cmt/details/wwLink.hpp>

namespace dci::cmt::details
{
    class API_DCI_CMT Waiter final
        : public himpl::FaceLayout<Waiter, impl::details::Waiter>
    {
        Waiter(const Waiter&) = delete;
        void operator=(const Waiter&) = delete;

    public:
        using ExprEvaluator = bool(*)(void* eeData);

    public:
        Waiter(WWLink* links, std::size_t amount);
        Waiter(WWLink* links, std::size_t amount, void(*cb)(void* cbData), void* cbData);
        ~Waiter();

        void any(std::size_t* acquiredIndex);
        void all();
        void expr(ExprEvaluator ee, void* eeData, std::byte* bitsInBytes);

        void reset();
    };

}
