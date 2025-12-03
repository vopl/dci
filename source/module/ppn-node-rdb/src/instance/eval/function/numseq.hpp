// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "exec.hpp"
#include "../../eval.hpp"
#include "utils/numCommonType.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    namespace numseq
    {
        template <class T>  struct isSeqHelper          : std::false_type   {};
        template <class T>  struct isSeqHelper<List<T>> : std::true_type    {};
        template <class T>  struct isSeqHelper<Set<T>>  : std::true_type    {};

        template <class T>  struct isSeq : isSeqHelper<std::decay_t<T>>    {};

        pql::Value execImpl(const Args& args, const auto& processor)
        {
            if(args.size() != 1)
            {
                return pql::Value{};
            }

            return args[0].data.visit([&processor](const auto& valuesSeq)
            {
                if constexpr(!numseq::isSeq<decltype(valuesSeq)>())
                {
                    return pql::Value{};
                }
                else
                {
                    return numCommonType::exec(valuesSeq, valuesSeq.size(), [&valuesSeq, &processor](auto* targetPtrStub)
                    {

                        using T = std::remove_pointer_t<decltype(targetPtrStub)>;
                        if constexpr(!std::is_arithmetic_v<T>)
                        {
                            return pql::Value{};
                        }
                        else
                        {
                            List<T> casted;
                            for(const pql::Value& v : valuesSeq)
                            {
                                casted.emplace_back(cast<T>(v));
                            }

                            return processor(casted);
                        }
                    });
                }
            });
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::numseq_max>(const Args& args)
    {
        return numseq::execImpl(args, [](const auto& list) -> pql::Value
        {
            using T = typename std::decay_t<decltype(list)>::value_type;

            if(list.empty())
            {
                return pql::Value{std::numeric_limits<T>::lowest()};
            }

            return pql::Value{std::accumulate(list.begin(), list.end(), std::numeric_limits<T>::lowest(), [](const T& a, const T& b){
                    return std::max(a, b);
                })};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::numseq_min>(const Args& args)
    {
        return numseq::execImpl(args, [](const auto& list)
        {
            using T = typename std::decay_t<decltype(list)>::value_type;

            if(list.empty())
            {
                return pql::Value{std::numeric_limits<T>::max()};
            }

            return pql::Value{std::accumulate(list.begin(), list.end(), std::numeric_limits<T>::max(), [](const T& a, const T& b){
                    return std::min(a, b);
                })};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::numseq_avg>(const Args& args)
    {
        return numseq::execImpl(args, [](const auto& list)
        {
            using T = typename std::decay_t<decltype(list)>::value_type;

            if(list.empty())
            {
                return pql::Value{T{}};
            }

            T res = std::accumulate(list.begin(), list.end(), T{}) / static_cast<T>(list.size());

            return pql::Value{res};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::numseq_median>(const Args& args)
    {
        return numseq::execImpl(args, [](const auto& list)
        {
            using T = typename std::decay_t<decltype(list)>::value_type;

            if(list.empty())
            {
                return pql::Value{T{}};
            }

            auto clone = list;
            std::sort(clone.begin(), clone.end());

            return pql::Value{clone[clone.size()/2]};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::numseq_sum>(const Args& args)
    {
        return numseq::execImpl(args, [](const auto& list)
        {
            using T = typename std::decay_t<decltype(list)>::value_type;

            if(list.empty())
            {
                return pql::Value{T{}};
            }

            return pql::Value{std::accumulate(list.begin(), list.end(), T{})};
        });
    }
}
