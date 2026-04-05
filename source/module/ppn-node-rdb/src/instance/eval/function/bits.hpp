/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "exec.hpp"
#include "../../eval.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    namespace bits
    {
        class Args
            : public function::Args
        {
        public:
            Args(const function::Args& from)
                : function::Args(from)
            {
            }

            decltype(auto) detectResultType(std::size_t argsCount, const auto& sink)
            {
                bool hasBool = false;
                bool hasUint = false;
                bool hasInt = false;
                bool hasId = false;

                std::size_t minSize = std::numeric_limits<std::size_t>::max();
                std::size_t maxSize = 0;

                for(std::size_t argIdx(0); argIdx<argsCount; ++argIdx)
                {
                    operator[](argIdx).data.visit([&](const auto& v)
                    {
                        using T = std::decay_t<decltype(v)>;
                        std::size_t size = 0;

                        if constexpr(std::is_same_v<bool, T>)
                        {
                            hasBool = true;
                            size = 1;
                        }
                        else if constexpr(std::is_same_v<pql::id128, T> || std::is_same_v<pql::id256, T> || std::is_same_v<pql::id512, T>)
                        {
                            hasId = true;
                            size = sizeof(T)*8;
                        }
                        else if constexpr(std::is_integral_v<T>)
                        {
                            if constexpr(std::is_signed_v<T>)
                            {
                                hasInt = true;
                            }
                            else
                            {
                                hasUint = true;
                            }
                            size = sizeof(T)*8;
                        }
                        else
                        {
                            return;
                        }

                        minSize = std::min(size, minSize);
                        maxSize = std::max(size, maxSize);
                    });
                }

                if(hasId)
                {
                    if(maxSize >= 512)
                    {
                        return sink(std::add_pointer_t<pql::id512>{});
                    }
                    if(maxSize >= 256)
                    {
                        return sink(std::add_pointer_t<pql::id256>{});
                    }
                    return sink(std::add_pointer_t<pql::id128>{});
                }

                if(hasUint)
                {
                    if(maxSize >= 64)
                    {
                        return sink(std::add_pointer_t<uint64>{});
                    }
                    if(maxSize >= 32)
                    {
                        return sink(std::add_pointer_t<uint32>{});
                    }
                    if(maxSize >= 16)
                    {
                        return sink(std::add_pointer_t<uint16>{});
                    }
                    return sink(std::add_pointer_t<uint8>{});
                }

                if(hasInt)
                {
                    if(maxSize >= 64)
                    {
                        return sink(std::add_pointer_t<int64>{});
                    }
                    if(maxSize >= 32)
                    {
                        return sink(std::add_pointer_t<int32>{});
                    }
                    if(maxSize >= 16)
                    {
                        return sink(std::add_pointer_t<int16>{});
                    }
                    return sink(std::add_pointer_t<int8>{});
                }

                if(hasBool)
                {
                    return sink(std::add_pointer_t<bool_>{});
                }

                return sink(std::add_pointer_t<void>{});
            }
        };

        template <bool init, bool reduce, class Target>
        pql::Value applyCombiner(const Args& args, const auto& combiner)
        {
            Target res = cast<Target>(args[0]);

            if constexpr(init)
            {
                combiner(res);
            }

            if constexpr(reduce)
            {
                for(std::size_t argIdx(1); argIdx<args.size(); ++argIdx)
                {
                    combiner(res, cast<Target>(args[argIdx]));
                }
            }

            return {res};
        }

        template <bool init, bool reduce>
        pql::Value applyCombiner(bits::Args concreteArgs, std::size_t argsAmount, const auto& combiner)
        {
            return concreteArgs.detectResultType(argsAmount, [&](auto* targetPtrStub) -> pql::Value
            {
                if constexpr(std::is_same_v<void*, decltype(targetPtrStub)>)
                {
                    return {};
                }
                else
                {
                    return applyCombiner<init, reduce, std::remove_pointer_t<decltype(targetPtrStub)>>(concreteArgs, combiner);
                }
            });
        }

        pql::Value applyCombinerEq1(const Args& args, const auto& combiner)
        {
            if(args.size() != 1) return {};
            return applyCombiner<true, false>(bits::Args{args}, 1, combiner);
        }

        pql::Value applyCombinerGe2(const Args& args, const auto& combiner)
        {
            if(args.size() < 2) return {};
            return applyCombiner<false, true>(bits::Args{args}, args.size(), combiner);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /// neg
    template <>
    pql::Value exec<pql::fun::bits_neg>(const Args& args)
    {
        return bits::applyCombinerEq1(args, [](auto& res)
        {
            using Target = std::decay_t<decltype(res)>;

            if constexpr(std::is_same_v<Target, bool>)
            {
                res = !res;
            }
            else if constexpr(std::is_same_v<Target, pql::id128> || std::is_same_v<Target, pql::id256> || std::is_same_v<Target, pql::id512>)
            {
                for(std::size_t idx(0); idx < res.data.size(); ++idx)
                {
                    res.data[idx] = ~res.data[idx];
                }
            }
            else
            {
                res = ~res;
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /// and
    template <>
    pql::Value exec<pql::fun::bits_and>(const Args& args)
    {
        return bits::applyCombinerGe2(args, [](auto& res, const auto& next)
        {
            using Target = std::decay_t<decltype(res)>;

            if constexpr(std::is_same_v<Target, bool>)
            {
                res = res && next;
            }
            else if constexpr(std::is_same_v<Target, pql::id128> || std::is_same_v<Target, pql::id256> || std::is_same_v<Target, pql::id512>)
            {
                for(std::size_t idx(0); idx < res.data.size(); ++idx)
                {
                    res.data[idx] = res.data[idx] & next.data[idx];
                }
            }
            else
            {
                res = res & next;
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /// or
    template <>
    pql::Value exec<pql::fun::bits_or>(const Args& args)
    {
        return bits::applyCombinerGe2(args, [](auto& res, const auto& next)
        {
            using Target = std::decay_t<decltype(res)>;

            if constexpr(std::is_same_v<Target, bool>)
            {
                res = res || next;
            }
            else if constexpr(std::is_same_v<Target, pql::id128> || std::is_same_v<Target, pql::id256> || std::is_same_v<Target, pql::id512>)
            {
                for(std::size_t idx(0); idx < res.data.size(); ++idx)
                {
                    res.data[idx] = res.data[idx] | next.data[idx];
                }
            }
            else
            {
                res = res | next;
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /// xor
    template <>
    pql::Value exec<pql::fun::bits_xor>(const Args& args)
    {
        return bits::applyCombinerGe2(args, [](auto& res, const auto& next)
        {
            using Target = std::decay_t<decltype(res)>;

            if constexpr(std::is_same_v<Target, bool>)
            {
                res = !res != !next;
            }
            else if constexpr(std::is_same_v<Target, pql::id128> || std::is_same_v<Target, pql::id256> || std::is_same_v<Target, pql::id512>)
            {
                for(std::size_t idx(0); idx < res.data.size(); ++idx)
                {
                    res.data[idx] = res.data[idx] ^ next.data[idx];
                }
            }
            else
            {
                res = res ^ next;
            }
        });
    }

}
