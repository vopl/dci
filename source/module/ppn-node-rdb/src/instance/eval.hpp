// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include <type_traits>
#include <charconv>

namespace dci::module::ppn::node::rdb::instance::table
{
    class Record;
}

namespace dci::module::ppn::node::rdb::instance::eval
{
    template <class T> T cast(const pql::Value& v);
    pql::Expression combine(const pql::Expression& expr, const table::Record& record);
}

namespace dci::module::ppn::node::rdb::instance::eval
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> T cast(const pql::Value& v)
    {
        return v.data.visit([](const auto& s) -> T
        {
            using S = std::decay_t<decltype(s)>;
            if constexpr(std::is_convertible_v<S, T>)
            {
                return static_cast<T>(s);
            }
            else if constexpr(std::is_same_v<S, String>)
            {
                if constexpr(std::is_same_v<T, bool>)
                {
                    return !s.empty();
                }
                else if constexpr(std::is_integral_v<T>)
                {
                    T res{};
                    std::from_chars(s.data(), s.data()+s.size(), res);
                    return res;
                }
                else if constexpr(std::is_floating_point_v<T>)
                {
                    try
                    {
                        return static_cast<T>(stold(s));
                    }
                    catch(...)
                    {

                    }
                    return T{};
                }
                else
                {
                    return T{};
                }
            }
            else if constexpr(std::is_same_v<T, String>)
            {
                if constexpr(std::is_same_v<S, bool>)
                {
                    return s?"1":"";
                }
                else if constexpr(std::is_arithmetic_v<S>)
                {
                    return std::to_string(s);
                }
                else
                {
                    return T{};
                }
            }
            else
            {
                return T{};
            }
        });
    }
}
