// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "source.hpp"
#include "../serialization.hpp"
#include "serialization.hpp"

namespace dci::stiac::link
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source& Source::operator>>(auto&& v)
    {
        using stiac::serialization::load;
        using stiac::link::serialization::load;

        load(*this, std::forward<decltype(v)>(v));
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    SourceZombie<T> Source::makeZombie(const T& v)
    {
        return SourceZombie<T>(this, v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    SourceZombie<T>::SourceZombie(Source* source, const T& v)
        : _source(source)
        , _v(v)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void SourceZombie<T>::pushPtr(std::type_index typeIndex, Ptr<void> v)
    {
        return _source->pushPtr(typeIndex, std::move(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    std::pair<std::type_index, Ptr<void>> SourceZombie<T>::getPtr(uint32 idx)
    {
        return _source->getPtr(idx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool SourceZombie<T>::mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid)
    {
        return _source->mapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool SourceZombie<T>::unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid)
    {
        return _source->unmapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void SourceZombie<T>::fail(const char* cszDetails)
    {
        return _source->fail(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool SourceZombie<T>::emplaceLink(BasePtr&& link, RemoteId id)
    {
        return _source->emplaceLink(std::move(link), id);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    SourceZombie<T>& SourceZombie<T>::operator>>(T& v)
    {
        v = _v;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    SourceZombie<T>& SourceZombie<T>::operator>>(auto&& v)
    {
        using stiac::serialization::load;
        using stiac::link::serialization::load;

        load(*this, std::forward<decltype(v)>(v));
        return *this;
    }

}
