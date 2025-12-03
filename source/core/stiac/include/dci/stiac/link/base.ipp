// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "base.hpp"
#include "sink.hpp"
#include "source.hpp"

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Base::Base()
        : _hub(nullptr)
        , _id(Id::null)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Base::initialize(Hub4Link* hub, Id id)
    {
        _hub = hub;
        _id = id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Base::deinitialize()
    {
        _hub = {};
        //_id = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Base::Deleter::operator()(Base* ptr) const
    {
        dbgAssert(ptr);
        ptr->destroy();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    R Base::call2Bin(MethodId methodId, Args&&... args)
    {
        if constexpr(!std::is_same_v<void, R>)
        {
            typename R::Opposite rPromise;
            R future = rPromise.future();

            if(_hub)
            {
                Sink sink = _hub->makeSink(_id);
                sink << methodId;
                (void)(sink << ... << std::forward<Args>(args));
                sink << std::move(rPromise);
                sink.finalize();
            }
            else
            {
                rPromise.resolveCancel();
            }

            return future;
        }
        else
        {
            if(_hub)
            {
                Sink sink = _hub->makeSink(_id);
                sink << methodId;
                (void)(sink << ... << std::forward<Args>(args));
                sink.finalize();
            }
            else
            {
                //ignore
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    void Base::bin2Call(Source& source, auto&& sink)
    {
        Tuple<Args...> args;
        source >> args;

        if constexpr(!std::is_same_v<void, R>)
        {
            RemoteId resId;
            source >> resId;
            source.finalize();

            source.makeZombie(resId) >> std::move(args).apply([&](Args&&... args){return std::forward<decltype(sink)>(sink)(std::move(args)...);});
        }
        else
        {
            source.finalize();

            std::move(args).apply([&](Args&&... args){return std::forward<decltype(sink)>(sink)(std::move(args)...);});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    void Base::bin2Call(Source& source, sbs::Wire<R, Args...>& sink)
    {
        return bin2Call<R, Args...>(source, [&](Args&&... args) -> R
        {
            return sink.in(std::move(args)...);
        });
    }

}
