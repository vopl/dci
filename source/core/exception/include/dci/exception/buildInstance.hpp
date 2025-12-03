// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include "../eid.hpp"
#include <exception>
#include <string>

namespace dci::exception
{
    std::exception_ptr API_DCI_EXCEPTION buildInstance(const Eid& eid, const std::exception_ptr& cause = std::exception_ptr());

    template <class E, class... Args>
    std::exception_ptr buildInstance(Args&&... args) requires std::is_constructible_v<E, Args&&...>;

    template <class E, class... Args>
    std::exception_ptr buildInstance(const std::exception_ptr& cause, Args&&... args) requires std::is_constructible_v<E, Args&&...>;
}

namespace dci::exception
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    std::exception_ptr buildInstance(Args&&... args) requires std::is_constructible_v<E, Args&&...>
    {
        return std::make_exception_ptr(E{std::forward<Args>(args)...});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E, class... Args>
    std::exception_ptr buildInstance(const std::exception_ptr& cause, Args&&... args) requires std::is_constructible_v<E, Args&&...>
    {
        if(cause)
        {
            try
            {
                try { std::rethrow_exception(cause); }
                catch (...)
                {
                    std::throw_with_nested(E{std::forward<Args>(args)...});
                }
            }
            catch(...)
            {
                return std::current_exception();
            }
        }

        return buildInstance<E>(std::forward<Args>(args)...);
    }

}
