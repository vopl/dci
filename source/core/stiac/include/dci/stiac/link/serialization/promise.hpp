// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/promise.hpp>
#include "../promise.hpp"

namespace dci::stiac::link::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, cmt::Promise<T>&& v)
    {
        dbgAssert(v.charged());
        ar << ar.emplaceLink(BasePtr(new  Impl<cmt::Promise<T>>{std::move(v)}));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, cmt::Promise<T>&& v)
    {
        dbgAssert(v);
        RemoteId remoteId;
        ar >> remoteId;

        BasePtr rLink(new  Impl<cmt::Promise<T>>{std::move(v)});
        if(!ar.emplaceLink(rLink, remoteId))
        {
            ar.fail("bad link");
        }
    }
}
