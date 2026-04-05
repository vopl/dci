/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../descriptor.hpp"
#include "../clocking/config.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <deque>
#include <atomic>
#include <windows.h>

namespace bmi = boost::multi_index;

namespace dci::poll::impl::polling
{
    class AsyncSelect
    {
    public:
        AsyncSelect();
        ~AsyncSelect();

        std::error_code initialize();

        bool initialized() const;

        std::error_code installDescriptor(Descriptor* d);
        std::error_code uninstallDescriptor(Descriptor* d);

        std::error_code execute(clocking::Duration timeout);
        std::error_code wakeup();

        std::error_code deinitialize();

    private:
        LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HWND _hwnd{NULL};
        HANDLE _hWakeupEvent{NULL};
        std::atomic_bool _wakeup{};
        struct Element
        {
            Descriptor* _descriptor{};
            SOCKET      _socket{};
            Element(Descriptor* descriptor, SOCKET socket)
                : _descriptor{descriptor}
                , _socket{socket}
            {}
        };
        using ByDescriptor = bmi::member<Element, Descriptor*, &Element::_descriptor>;
        using BySocket = bmi::member<Element, SOCKET, &Element::_socket>;

        using Registry = bmi::multi_index_container<
            Element,
            bmi::indexed_by<
                bmi::ordered_unique<bmi::tag<ByDescriptor>, ByDescriptor>,
                bmi::ordered_unique<bmi::tag<BySocket>, BySocket>>>;

        Registry            _registry;

        std::deque<MSG>     _messages;
    };
}
