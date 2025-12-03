// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include <typeindex>

namespace dci::stiac::serialization
{
    enum class PtrStoreKind : uint8
    {
        null  = 0,
        value = 1,
        share = 2,
    };

    using PtrValueIndex = uint32;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, const Ptr<T>& v)
    {
        if(!v)
        {
            ar << PtrStoreKind::null;
            return;
        }

        auto [idx, isNew] = ar.pushPtr(v.get());
        if(isNew)
        {
            ar << PtrStoreKind::value;
            ar << *v;
            return;
        }

        ar << PtrStoreKind::share;
        ar << idx;
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, Ptr<T>&& v)
    {
        if(!v)
        {
            ar << PtrStoreKind::null;
            return;
        }

        auto [idx, isNew] = ar.pushPtr(v.get());
        if(isNew)
        {
            ar << PtrStoreKind::value;
            ar << std::move(*v);
            v.reset();
            return;
        }

        ar << PtrStoreKind::share;
        ar << idx;
        v.reset();
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, Ptr<T>& v)
    {
        PtrStoreKind ptrStoreKind;
        ar >> ptrStoreKind;

        switch(ptrStoreKind)
        {
        case PtrStoreKind::null:
            v.reset();
            return;

        case PtrStoreKind::value:
            v.reset(new T);
            ar.pushPtr(std::type_index(typeid(T)), v);
            ar >> *v;
            return;

        case PtrStoreKind::share:
            {
                PtrValueIndex idx;
                ar >> idx;

                auto [typeIndex, ptr] = ar.getPtr(idx);

                if(typeIndex != std::type_index(typeid(T)))
                {
                    ar.fail("malformed input stream (corrupted data)");
                }

                v = std::static_pointer_cast<T>(ptr);
            }
            return;
        }

        ar.fail("malformed input stream (corrupted data)");
    }
}
