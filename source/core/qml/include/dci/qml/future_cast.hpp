// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <dci/cmt/future.hpp>
#include <dci/cmt/promise.hpp>

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct FutureCastImpl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct FutureCastImpl<cmt::Future<To>, cmt::Future<From>>
    {
        static cmt::Future<To> exec(const cmt::Future<From>& from)
        {
            return from.chain() += [](const cmt::Future<From>& in, cmt::Promise<To>& out)
            {
                if(in.resolvedCancel())
                {
                    out.resolveCancel();
                    return;
                }

                if(in.resolvedException())
                {
                    out.resolveException(in.exception());
                    return;
                }

                out.resolve(value_cast<To>(in.value()));
            };
        }
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct FutureCastImpl<cmt::Future<To>, From>
    {
        static cmt::Future<To> exec(const From& from)
        {
            (void)from;
            dbgFatal("not impl");
            return {};
        }
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    To future_cast(const From& from)
    {
        return FutureCastImpl<To, From>::exec(from);
    }
}

#include "value_cast.hpp"
