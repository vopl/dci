// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "exec.hpp"
#include "../../eval.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class SeqArgs
        : public Args
    {
    public:
        const pql::Expression& getRaw(std::size_t idx) const
        {
            dbgAssert(idx < size());
            return _raw[idx];
        }

        pql::Value getRawCombined(std::size_t idx) const
        {
            dbgAssert(idx < size());
            return combine(_raw[idx], _record).getOr(_nullStub);
        }
    };

    namespace seq
    {
        template <class T>  struct isSeqHelper          : std::false_type   {};
        template <class T>  struct isSeqHelper<List<T>> : std::true_type    {};
        template <class T>  struct isSeqHelper<Set<T>>  : std::true_type    {};

        template <class T>  struct isSeq : isSeqHelper<std::decay_t<T>>    {};

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Seq, class E> void addElement(Seq& seq, E&& element)
        {
            if constexpr(std::is_same_v<List<pql::Value>, Seq>)
            {
                seq.emplace_back(std::forward<decltype(element)>(element));
            }
            else if constexpr(std::is_same_v<Set<pql::Value>, Seq>)
            {
                seq.emplace(std::forward<decltype(element)>(element));
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        pql::Value execImpl(const Args& args, const auto& f)
        {
            if(args.size() < 2)
            {
                return pql::Value{};
            }

            SeqArgs seqArgs{args};

            const pql::Expression& processor = seqArgs.getRaw(0);
            if(!processor.holds<pql::Node>())
            {
                return pql::Value{};
            }

            const pql::Value& varSeq = seqArgs.getRawCombined(1);

            return varSeq.data.visit([&](const auto& seq)
            {
                if constexpr(!seq::isSeq<decltype(seq)>())
                {
                    return pql::Value{};
                }
                else
                {
                    pql::Node processorNode = processor.get<pql::Node>();
                    return f(processorNode, seq, args.record());
                }
            });
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::seq_map>(const Args& args)
    {
        return seq::execImpl(args, [](pql::Node& processorNode, const auto& seq, const table::Record& record) -> pql::Value
        {
            std::decay_t<decltype(seq)> res;

            processorNode.args.emplace_back(pql::Value{});
            for(const auto& v : seq)
            {
                processorNode.args.back() = v;
                seq::addElement(res, combine(processorNode, record).getOr(pql::Value{}));
            }

            return {res};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::seq_grep>(const Args& args)
    {
        return seq::execImpl(args, [](pql::Node& processorNode, const auto& seq, const table::Record& record)
        {
            std::decay_t<decltype(seq)> res;

            processorNode.args.emplace_back(pql::Value{});
            for(const auto& v : seq)
            {
                processorNode.args.back() = v;
                if(eval::cast<bool>(combine(processorNode, record).getOr(pql::Value{})))
                {
                    seq::addElement(res, v);
                }
            }

            return pql::Value{res};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::seq_reduce>(const Args& args)
    {
        return seq::execImpl(args, [](pql::Node& processorNode, const auto& seq, const table::Record& record)
        {
            if(seq.empty())
            {
                return pql::Value{};
            }

            if(1 == seq.size())
            {
                return *seq.begin();
            }

            processorNode.args.emplace_back(pql::Value{});
            processorNode.args.emplace_back(pql::Value{});

            auto iter {seq.begin()};
            pql::Value res = *iter;
            ++iter;
            for(; iter != seq.end(); ++iter)
            {
                processorNode.args[0] = res;
                processorNode.args[1] = *iter;
                res = combine(processorNode, record).getOr(pql::Value{});
            }

            return res;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::seq_at>(const Args& args)
    {
        if(args.size() != 2)
        {
            return pql::Value{};
        }

        std::size_t targetIndex = eval::cast<std::size_t>(args[1]);

        return args[0].data.visit([&](const auto& seq)
        {
            if constexpr(!seq::isSeq<decltype(seq)>())
            {
                return pql::Value{};
            }
            else
            {
                if(targetIndex >= seq.size())
                {
                    return pql::Value{};
                }
                return pql::Value{*std::next(seq.begin(), static_cast<std::ptrdiff_t>(targetIndex))};
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::seq_size>(const Args& args)
    {
        if(args.size() != 1)
        {
            return pql::Value{};
        }

        return args[0].data.visit([&](const auto& seq)
        {
            if constexpr(!seq::isSeq<decltype(seq)>())
            {
                return pql::Value{};
            }
            else
            {
                return pql::Value{seq.size()};
            }
        });
    }
}
