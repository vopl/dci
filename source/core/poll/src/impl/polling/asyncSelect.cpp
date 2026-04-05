/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "asyncSelect.hpp"
#include <dci/poll/descriptor.hpp>
#include <dci/poll/error.hpp>
#include <dci/utils/win32/error.hpp>

namespace dci::poll::impl::polling
{
    namespace
    {
        AsyncSelect* instance{};
    }
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    AsyncSelect::AsyncSelect()
    {
        dbgAssert(!instance);
        instance = this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    AsyncSelect::~AsyncSelect()
    {
        wakeup();
        deinitialize();
        dbgAssert(this == instance);
        instance = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code AsyncSelect::initialize()
    {
        if(NULL != _hwnd)
        {
            return error::already_initialized;
        }

        WSADATA wsaData;
        if(int err = WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            return utils::win32::error::make(err);
        }

        if(MAKEWORD(2, 2) != wsaData.wVersion)
        {
            WSACleanup();
            return utils::win32::error::make(ERROR_NOT_SUPPORTED);
        }

        _hWakeupEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
        if(!_hWakeupEvent)
        {
            WSACleanup();
            return utils::win32::error::last();
        }

        const wchar_t wndClassName[]  = L"dciPollingAsyncSelectorWindow";
        static std::error_code registerClassEc = [&]
        {
            WNDCLASSW wc{};
            wc.lpfnWndProc   = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
            {
                if(!instance)
                {
                    return ::DefWindowProcW(hwnd, msg, wParam, lParam);
                }

                return instance->wndProc(hwnd, msg, wParam, lParam);
            };

            wc.lpszClassName = wndClassName;
            if(!::RegisterClassW(&wc))
            {
                return utils::win32::error::last();
            }

            return std::error_code{};
        }();

        if(registerClassEc)
        {
            WSACleanup();
            return registerClassEc;
        }

        _hwnd = ::CreateWindowExW(
            0,              //[in]           DWORD     dwExStyle,
            wndClassName,   //[in, optional] LPCWSTR   lpClassName,
            nullptr,        //[in, optional] LPCWSTR   lpWindowName,
            0,              //[in]           DWORD     dwStyle,
            CW_USEDEFAULT,  //[in]           int       X,
            CW_USEDEFAULT,  //[in]           int       Y,
            CW_USEDEFAULT,  //[in]           int       nWidth,
            CW_USEDEFAULT,  //[in]           int       nHeight,
            HWND_MESSAGE,   //[in, optional] HWND      hWndParent,
            nullptr,        //[in, optional] HMENU     hMenu,
            nullptr,        //[in, optional] HINSTANCE hInstance,
            nullptr         //[in, optional] LPVOID    lpParam
        );

        if(NULL == _hwnd)
        {
            WSACleanup();
            return utils::win32::error::last();
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool AsyncSelect::initialized() const
    {
        return NULL != _hwnd;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code AsyncSelect::installDescriptor(Descriptor* d)
    {
#define WM_SOCKET_SELECTED (WM_USER+1)

        auto [iter, inserted] = _registry.emplace(d, d->native());
        if(!inserted)
        {
            return make_error_code(error::already_installed);
        }

        if(0 != ::WSAAsyncSelect(
                    d->native(),
                    _hwnd,
                    WM_SOCKET_SELECTED,
                    FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE))
        {
            _registry.erase(iter);
            return utils::win32::error::make(::WSAGetLastError());
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code AsyncSelect::uninstallDescriptor(Descriptor* d)
    {
        auto& idx = _registry.get<ByDescriptor>();
        auto iter = idx.find(d);
        if(idx.end() == iter)
        {
            return make_error_code(error::not_installed);
        }

        SOCKET socket = iter->_socket;

        if(0 != ::WSAAsyncSelect(
               socket,
               _hwnd,
               0,
               0))
        {
            _registry.erase(iter);
            return utils::win32::error::make(::WSAGetLastError());
        }

        _registry.erase(iter);

        MSG msg{};
        while(::PeekMessageW(&msg, _hwnd, 0, 0, PM_REMOVE))
        {
            if(msg.wParam != socket)
            {
                _messages.push_back(msg);
            }
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code AsyncSelect::execute(clocking::Duration timeout)
    {
        if(!_messages.empty())
        {
            while(!_wakeup.load(std::memory_order_acquire) && !_messages.empty())
            {
                ::TranslateMessage(&_messages.front());
                ::DispatchMessageW(&_messages.front());
                _messages.pop_front();
            }
        }
        else
        {
            int timeoutMs;
            if(timeout.count() < 0)
            {
                timeoutMs = -1;
            }
            else
            {
                timeoutMs = static_cast<int>(std::chrono::ceil<std::chrono::milliseconds>(timeout).count());
            }

            if(!_wakeup.load(std::memory_order_acquire))
            {
                MSG msg{};
                if(!::PeekMessageW(&msg, _hwnd, 0, 0, PM_NOREMOVE))
                {
                    ::MsgWaitForMultipleObjects(
                                1,
                                &_hWakeupEvent,
                                FALSE,
                                timeoutMs,
                                QS_ALLINPUT);
                }

                while(!_wakeup.load(std::memory_order_acquire) && ::PeekMessageW(&msg, _hwnd, 0, 0, PM_REMOVE))
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessageW(&msg);
                }
            }
        }

        if(_wakeup.exchange(false, std::memory_order_acq_rel))
        {
            return make_error_code(std::errc::interrupted);
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code AsyncSelect::wakeup()
    {
        _wakeup.store(true, std::memory_order_release);

        if(!::SetEvent(_hWakeupEvent))
        {
            return dci::utils::win32::error::last();
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code AsyncSelect::deinitialize()
    {
        if(NULL == _hwnd)
        {
            return error::not_initialized;
        }

        ::DestroyWindow(_hwnd);
        _hwnd = NULL;

        CloseHandle(_hWakeupEvent);
        _hWakeupEvent = NULL;

        ::WSACleanup();

        return {};
    }

    LRESULT AsyncSelect::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if(WM_SOCKET_SELECTED == msg)
        {
            SOCKET socket = wParam;
            auto& idx = _registry.get<BySocket>();
            auto iter = idx.find(socket);
            if(idx.end() != iter)
            {
                WORD event = WSAGETSELECTEVENT(lParam);
                WORD error = WSAGETSELECTERROR(lParam);

                using RSF = descriptor::ReadyStateFlags;
                RSF readyState =
                        ((event & (FD_READ|FD_ACCEPT  ))    ? RSF::rsf_read  : RSF::rsf_null) |
                        ((event & (FD_OOB             ))    ? RSF::rsf_pri   : RSF::rsf_null) |
                        ((event & (FD_WRITE|FD_CONNECT))    ? RSF::rsf_write : RSF::rsf_null) |
                        ((event & (FD_CLOSE           ))    ? RSF::rsf_eof   : RSF::rsf_null) |
                        ((error                        )    ? RSF::rsf_error : RSF::rsf_null) |
                        RSF::rsf_null;

                Descriptor* d = iter->_descriptor;
                d->setReadyState(readyState);
            }

            return 0;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
