// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>

namespace dci::stiac::serialization
{
    enum class OptStoreKind : uint8
    {
        null  = 0,
        value = 1,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, const Opt<T>& v)
    {
        if(!v)
        {
            ar << OptStoreKind::null;
            return;
        }

        ar << OptStoreKind::value;
        ar << *v;
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, Opt<T>&& v)
    {
        if(!v)
        {
            ar << OptStoreKind::null;
            return;
        }

        ar << OptStoreKind::value;
        ar << std::move(*v);
        v.reset();
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, Opt<T>& v)
    {
        OptStoreKind optStoreKind;
        ar >> optStoreKind;

        switch(optStoreKind)
        {
        case OptStoreKind::null:
            v.reset();
            return;

        case OptStoreKind::value:
            if(!v)
            {
                v.emplace();
            }
            ar >> *v;
            return;
        }

        ar.fail("malformed input stream (corrupted data)");
    }
}
