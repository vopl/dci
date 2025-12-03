// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/future.hpp>
#include "../future.hpp"

namespace dci::stiac::link::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, const cmt::Future<T>& v)
    {
        dbgAssert(v.charged());
        ar << ar.emplaceLink(new Impl<cmt::Future<T>>{v});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, cmt::Future<T>&& v)
    {
        dbgAssert(v.charged());
        ar << ar.emplaceLink(new Impl<cmt::Future<T>>{std::move(v)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, const cmt::Future<T>& v)
    {
        dbgAssert(v.charged());

        RemoteId remoteId;
        ar >> remoteId;

        if(!ar.emplaceLink(BasePtr(new Impl<cmt::Future<T>>{v}), remoteId))
        {
            ar.fail("bad link");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, cmt::Future<T>&& v)
    {
        dbgAssert(v.charged());

        RemoteId remoteId;
        ar >> remoteId;

        if(!ar.emplaceLink(BasePtr(new Impl<cmt::Future<T>>{std::move(v)}), remoteId))
        {
            ar.fail("bad link");
        }
    }

}
