/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "api.hpp"
#include "../eid.hpp"
#include <exception>
#include <string>

namespace dci::exception
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    [[noreturn]] void API_DCI_EXCEPTION throwInstance(const Eid& eid, const std::exception_ptr& cause = {});

    std::exception_ptr API_DCI_EXCEPTION buildInstance(const Eid& eid, const std::exception_ptr& cause = {});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    [[noreturn]] void throwInstance(Args&&... args) requires std::is_constructible_v<E, Args&&...>;

    template <class E, class... Args>
    std::exception_ptr buildInstance(Args&&... args) requires std::is_constructible_v<E, Args&&...>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    [[noreturn]] void throwInstance(const std::exception_ptr& cause, Args&&... args) requires std::is_constructible_v<E, Args&&...>;

    template <class E, class... Args>
    std::exception_ptr buildInstance(const std::exception_ptr& cause, Args&&... args) requires std::is_constructible_v<E, Args&&...>;
}

namespace dci::exception
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    [[noreturn]] void throwInstance(Args&&... args) requires std::is_constructible_v<E, Args&&...>
    {
        throw E{std::forward<Args>(args)...};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    std::exception_ptr buildInstance(Args&&... args) requires std::is_constructible_v<E, Args&&...>
    {
        try
        {
            throwInstance<E>(std::forward<Args>(args)...);
        }
        catch(...)
        {
            return std::current_exception();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    [[noreturn]] void throwInstance(const std::exception_ptr& cause, Args&&... args) requires std::is_constructible_v<E, Args&&...>
    {
        if(cause)
        {
            try
            {
                std::rethrow_exception(cause);
            }
            catch (...)
            {
                std::throw_with_nested(E{std::forward<Args>(args)...});
            }
        }

        throwInstance<E>(std::forward<Args>(args)...);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    std::exception_ptr buildInstance(const std::exception_ptr& cause, Args&&... args) requires std::is_constructible_v<E, Args&&...>
    {
        try
        {
            throwInstance<E>(cause, std::forward<Args>(args)...);
        }
        catch(...)
        {
            return std::current_exception();
        }
    }

}
