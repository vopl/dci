/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "descriptor.hpp"
#include "fiberPool.hpp"

#include <dci/utils/intrusiveDlist.hpp>

#ifdef _WIN32
#   include "polling/asyncSelect.hpp"
#else
#   include "polling/epoll.hpp"
#endif

#include <system_error>
#include <chrono>

namespace dci::poll::impl
{
    class Polling
    {
    public:
        Polling(FiberPool& fiberPool);
        ~Polling();

        std::error_code initialize();

        bool initialized() const;

        std::error_code installDescriptor(Descriptor* d);
        std::error_code uninstallDescriptor(Descriptor* d);

        std::error_code execute(clocking::Duration timeout);
        std::error_code wakeup();

        std::error_code deinitialize();

    public:
        bool hasPayload() const;

    private:
        sbs::Owner              _sol;
        FiberPool&              _fiberPool;

        utils::IntrusiveDlist<Descriptor, DescriptorTag4Polling>
                                _descriptors;
        utils::IntrusiveDlist<Descriptor, DescriptorTag4Ready>
                                _descriptorsReady;

#ifdef _WIN32
        polling::AsyncSelect    _engine;
#else
        polling::Epoll          _engine;
#endif
    };
}
