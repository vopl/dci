/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "promise.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "methodId.hpp"

namespace dci::stiac::link
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
     Impl<cmt::Promise<T>>:: Impl()
        : _promise()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
     Impl<cmt::Promise<T>>:: Impl(cmt::Promise<T>&& promise)
        : _promise(std::move(promise))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void  Impl<cmt::Promise<T>>::initialize(Hub4Link* hub, Id id)
    {
        Base::initialize(hub, id);

        _promise.canceled() += _sbsOwner * [&]
        {
            _sbsOwner.flush();

            if(!_hub)
            {
                return;
            }

            Sink sink = _hub->makeSink(_id);
            _hub->linkUninvolved(_id, Hub4Link::uf_beginRemove);

            sink << MethodId(0);
            sink.finalize();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void  Impl<cmt::Promise<T>>::input(Source& source)
    {
        _sbsOwner.flush();

        if(!_hub)
        {
            return;
        }

        cmt::Promise<T> promise(std::move(_promise));
        dbgAssert(promise.charged());
        dbgAssert(promise.resolvedCancel() || !promise.resolved());

        _hub->linkUninvolved(_id, Hub4Link::uf_remove | Hub4Link::uf_endRemove | Hub4Link::uf_sendEnd);

        bool canResolve = promise.charged() && !promise.resolved();

        MethodId methodId;
        source >> methodId;

        switch(static_cast<std::underlying_type_t<MethodId>>(methodId))
        {
        case 0://value
            {
                if constexpr(std::is_same_v<void, T>)
                {
                    source.finalize();

                    if(canResolve)
                    {
                        promise.resolveValue();
                    }
                }
                else
                {
                    T value;
                    source >> value;
                    source.finalize();

                    if(canResolve)
                    {
                        promise.resolveValue(std::move(value));
                    }
                }
            }
            break;

        case 1://exception
            {
                ExceptionPtr e;
                source >> e;
                source.finalize();

                if(canResolve)
                {
                    promise.resolveException(std::move(e));
                }
            }
            break;

        case 2://cancel
            {
                source.finalize();

                if(canResolve)
                {
                    promise.resolveCancel();
                }
            }
            break;

        default:
            source.fail("malformed input");
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void  Impl<cmt::Promise<T>>::destroy()
    {
        _sbsOwner.flush();

        if(_promise.charged() && !_promise.resolved())
        {
            _promise.resolveCancel();
        }

        delete this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    cmt::Future<T>  Impl<cmt::Promise<T>>::target()
    {
        dbgAssert(_promise);
        return _promise.future();
    }

}
