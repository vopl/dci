// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "exec.hpp"
#include "../../eval.hpp"
#include "utils/numCommonType.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    namespace num
    {
        template <std::size_t argsAmount, class Target>
        pql::Value applyImpl(const Args& args, const auto& impl)
        {
            if constexpr(0 == argsAmount)
            {
                return pql::Value{impl()};
            }
            else if constexpr(1 == argsAmount)
            {
                return pql::Value{impl(cast<Target>(args[0]))};
            }
            else if constexpr(2 == argsAmount)
            {
                return pql::Value{impl(cast<Target>(args[0]), cast<Target>(args[1]))};
            }
            else if constexpr(3 == argsAmount)
            {
                return pql::Value{impl(cast<Target>(args[0]), cast<Target>(args[1]), cast<Target>(args[2]))};
            }

            else if constexpr(4 == argsAmount)
            {
                return pql::Value{impl(cast<Target>(args[0]), cast<Target>(args[1]), cast<Target>(args[2]), cast<Target>(args[3]))};
            }
            else
            {
                dbgWarn("never here");
                return {};
            }
        }

        template <std::size_t argsAmount>
        pql::Value applyImpl(const Args& concreteArgs, const auto& impl)
        {
            return pql::Value{numCommonType::exec(concreteArgs.head(argsAmount), argsAmount, [&](auto* targetPtrStub)
            {
               if constexpr(std::is_same_v<void*, decltype(targetPtrStub)>)
               {
                   return pql::Value{};
               }
               else
               {
                   return applyImpl<argsAmount, std::remove_pointer_t<decltype(targetPtrStub)>>(concreteArgs, impl);
               }
            })};
        }

        pql::Value applyImplEq1(const Args& args, const auto& impl)
        {
            if(args.size() != 1) return {};
            return applyImpl<1>(args, impl);
        }

        pql::Value applyImplEq2(const Args& args, const auto& impl)
        {
            if(args.size() != 2) return {};
            return applyImpl<2>(args, impl);
        }

        pql::Value applyImplEq3(const Args& args, const auto& impl)
        {
            if(args.size() < 3) return {};
            return applyImpl<3>(args, impl);
        }
    }

    namespace
    {
        auto distance(const auto &a1, const auto &a2)
        {
            if(a1 > a2) return a1-a2;
            return a2-a1;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_eq>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return distance(a1, a2) <= a3;
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 == a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_ne>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return distance(a1, a2) > a3;
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 != a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_gt>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return (a1 - a2) > -a3;
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 > a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_lt>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return (a1 - a2) < a3;
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 < a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_ge>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return (a1 - a2) >= a3;
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 >= a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_le>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return (a1 - a2) <= a3;
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 <= a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_between>(const Args& args)
    {
        if(args.size() != 3) return {};

        return num::applyImpl<3>(args, [](const auto& a1, const auto& a2, const auto& a3)
        {
            return a2 <= a1 && a1 <= a3;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_neg>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            using T = std::decay_t<decltype(a1)>;

            if constexpr(std::is_same_v<T, bool>)
            {
                return -static_cast<int8>(a1);
            }
            else if constexpr(std::is_unsigned_v<T>)
            {
                return -static_cast<std::make_signed_t<T>>(a1);
            }
            else
            {
                return -a1;
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_add>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 + a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_sub>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 - a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_div>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            if(a2 == decltype(a2){})
            {
                return pql::Value{};
            }

            return pql::Value{a1 / a2};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_mul>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return a1 * a2;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_mod>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            if(a2 == decltype(a2){})
            {
                return pql::Value{};
            }

            if constexpr(std::is_floating_point_v<std::decay_t<decltype(a1)>>)
            {
                using CT = std::common_type_t<decltype(a1), decltype(a2)>;
                return pql::Value{std::fmod(static_cast<CT>(a1), static_cast<CT>(a2))};
            }
            else
            {
                return pql::Value{a1 % a2};
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_ceil>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::ceil(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_floor>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::floor(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_trunc>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::trunc(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_round>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::round(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_abs>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_unsigned_v<std::decay_t<decltype(a1)>>)
            {
                return a1;
            }
            else
            {
                return std::abs(a1);
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_log>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_signed_v<std::decay_t<decltype(a1)>>)
            {
                if(a1 < 0)
                {
                    return pql::Value{};
                }
            }

            return pql::Value{std::log(a1)};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_log10>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_signed_v<std::decay_t<decltype(a1)>>)
            {
                if(a1 < 0)
                {
                    return pql::Value{};
                }
            }

            return pql::Value{std::log10(a1)};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_log2>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_signed_v<std::decay_t<decltype(a1)>>)
            {
                if(a1 < 0)
                {
                    return pql::Value{};
                }
            }

            return pql::Value{std::log2(a1)};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_exp>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::exp(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_exp10>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return pow(10.0, static_cast<double>(a1));
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_exp2>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::exp2(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_pow>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return std::pow(a1, a2);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_sqrt>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_signed_v<std::decay_t<decltype(a1)>>)
            {
                if(a1 < 0)
                {
                    return pql::Value{};
                }
            }

            return pql::Value{std::sqrt(a1)};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_cbrt>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::cbrt(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_hypot>(const Args& args)
    {
        if(3 == args.size())
        {
            return num::applyImplEq3(args, [](const auto& a1, const auto& a2, const auto& a3)
            {
                return std::hypot(a1, a2, a3);
                //return std::sqrt(a1*a1 + a2*a2 + a3*a3);
            });
        }

        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return std::hypot(a1, a2);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_sin>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::sin(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_cos>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::cos(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_tan>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::tan(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_asin>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_signed_v<std::decay_t<decltype(a1)>>)
            {
                if(a1 < -1 || a1 > 1)
                {
                    return pql::Value{};
                }
            }
            else if constexpr(!std::is_same_v<std::decay_t<decltype(a1)>, bool>)
            {
                if(a1 > 1)
                {
                    return pql::Value{};
                }
            }

            return pql::Value{std::asin(a1)};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_acos>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            if constexpr(std::is_signed_v<std::decay_t<decltype(a1)>>)
            {
                if(a1 < -1 || a1 > 1)
                {
                    return pql::Value{};
                }
            }
            else if constexpr(!std::is_same_v<std::decay_t<decltype(a1)>, bool>)
            {
                if(a1 > 1)
                {
                    return pql::Value{};
                }
            }

            return pql::Value{std::acos(a1)};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_atan>(const Args& args)
    {
        return num::applyImplEq1(args, [](const auto& a1)
        {
            return std::atan(a1);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::num_atan2>(const Args& args)
    {
        return num::applyImplEq2(args, [](const auto& a1, const auto& a2)
        {
            return std::atan2(a1, a2);
        });
    }
}
