// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "future.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "methodId.hpp"

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Impl<cmt::Future<T>>::Impl(cmt::Future<T>&& future)
        : _future(std::move(future))
    {
        dbgAssert(_future.charged());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
     Impl<cmt::Future<T>>:: Impl(const cmt::Future<T>& future)
        : _future(future)
    {
        dbgAssert(_future.charged());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void  Impl<cmt::Future<T>>::initialize(Hub4Link* hub, Id id)
    {
        Base::initialize(hub, id);

        _future.then() += _sbsOwner * [&](cmt::Future<T>& future)
        {
            _sbsOwner.flush();

            if(!_hub)
            {
                return;
            }

            Sink sink = _hub->makeSink(_id);
            _hub->linkUninvolved(_id, Hub4Link::uf_beginRemove);

            dbgAssert(future.resolved());

            if(future.resolvedValue())
            {
                sink << MethodId(0);

                if constexpr(!std::is_same_v<void, T>)
                {
                    sink << future.detachValue();
                }
            }
            else if(future.resolvedException())
            {
                sink << MethodId(1);
                sink << future.detachException();
            }
            else // if(future.resolvedCancel())
            {
                sink << MethodId(2);
            }

            sink.finalize();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void  Impl<cmt::Future<T>>::input(Source& source)
    {
        _sbsOwner.flush();

        if(!_hub)
        {
            return;
        }

        cmt::Future<T> future(std::move(_future));
        dbgAssert(future.charged());

        _hub->linkUninvolved(_id, Hub4Link::uf_remove | Hub4Link::uf_endRemove | Hub4Link::uf_sendEnd);

        bool canResolve = future.charged() && !future.resolved();

        MethodId methodId;
        source >> methodId;

        switch(static_cast<std::underlying_type_t<MethodId>>(methodId))
        {
        case 0://cancel
            source.finalize();

            if(canResolve)
            {
                future.resolveCancel();
            }
            break;

        default:
            source.fail("malformed input");
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void  Impl<cmt::Future<T>>::destroy()
    {
        _sbsOwner.flush();

        if(_future.charged() && !_future.resolved())
        {
            _future.resolveCancel();
        }

        delete this;
    }

}
