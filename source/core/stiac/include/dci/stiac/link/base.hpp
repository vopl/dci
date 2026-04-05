/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/sbs/wire.hpp>
#include "id.hpp"
#include "methodId.hpp"
#include <memory>

namespace dci::stiac::link
{
    class Hub4Link;
    class Source;

    class Base
    {
        Base(const Base&) = delete;
        void operator=(const Base&) = delete;

    protected:
        Base();
        virtual ~Base() = default;

    public:
        virtual void initialize(Hub4Link* hub, Id id);
        virtual void deinitialize();
        virtual void input(Source& source) = 0;
        virtual void destroy() = 0;

    public:
        struct Deleter
        {
            void operator()(Base* ptr) const;
        };

    protected:
        template <class R, class... Args>
        R call2Bin(MethodId methodId, Args&&... args);

        template <class R, class... Args>
        void bin2Call(Source& source, auto&& sink);

        template <class R, class... Args>
        void bin2Call(Source& source, sbs::Wire<R, Args...>& sink);

    protected:
        Hub4Link*   _hub;
        Id          _id;
    };

    using BasePtr = std::unique_ptr<Base, Base::Deleter>;

}
