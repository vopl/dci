/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pipe.hpp"

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <winsock2.h>
#   include <afunix.h>
#   include <cstdio>
#else
#   include <unistd.h>
#endif

namespace utils
{
#ifdef _WIN32
    int pipe(dci::poll::descriptor::Native::Value sv[2])
    {
        SOCKET fd0 = INVALID_SOCKET;
        SOCKET fd1 = INVALID_SOCKET;
        SOCKET fdListen = INVALID_SOCKET;

        sockaddr_in sa{};
        int saLen = sizeof(sa);
        {
            sa.sin_family = AF_INET;
            sa.sin_port = 0;
            sa.sin_addr.S_un.S_un_b = {127, 231, 17, 85};
        }

        fdListen = ::socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
        if(fdListen == INVALID_SOCKET)
        {
            goto error;
        }

        if(bind(fdListen, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR)
        {
            goto error;
        }

        if(listen(fdListen, 5) == SOCKET_ERROR)
        {
            goto error;
        }

        fd0 = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
        if(fd0 == INVALID_SOCKET)
        {
            goto error;
        }

        {
            sa.sin_family = AF_INET;
            sa.sin_port = 0;
            sa.sin_addr.S_un.S_un_b = {127, 231, 17, 85};
        }
        if(bind(fd0, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR)
        {
            goto error;
        }

        if(getsockname(fdListen, (struct sockaddr*)&sa, &saLen) == SOCKET_ERROR)
        {
            goto error;
        }

        if(connect(fd0, (const struct sockaddr*)&sa, saLen) == SOCKET_ERROR)
        {
            goto error;
        }

        saLen = sizeof(sa);
        fd1 = accept(fdListen, (struct sockaddr*) & sa, &saLen);
        if((int)fd1 == SOCKET_ERROR)
        {
            goto error;
        }

        sv[0] = fd0;
        sv[1] = fd1;
        closesocket(fdListen);
        return 0;

error:
        int lastErrr = WSAGetLastError();
        if(fd0 != INVALID_SOCKET)
        {
            closesocket(fd0);
        }
        if(fd1 != INVALID_SOCKET)
        {
            closesocket(fd1);
        }
        if(fdListen != INVALID_SOCKET)
        {
            closesocket(fdListen);
        }
        WSASetLastError(lastErrr);
        return SOCKET_ERROR;
    }
#else
    int pipe(dci::poll::descriptor::Native::Value sv[2])
    {
        return ::pipe(sv);
    }
#endif
}
