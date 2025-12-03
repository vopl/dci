// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/idl/introspection.hpp>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept Struct = idl::introspection::isStruct<T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Struct T>
    void save(auto& ar, const T& v)
    {
        idl::introspection::applyBases([&](const auto&... v)
        {
            (void)(ar << ... << v);
        }, v);

        idl::introspection::applyFields([&](const auto&... v)
        {
            (void)(ar << ... << v);
        }, v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Struct T>
    void save(auto& ar, T&& v)
    {
        idl::introspection::applyBases([&](auto&&... v)
        {
            (void)(ar << ... << std::move(v));
        }, std::move(v));

        idl::introspection::applyFields([&](auto&&... v)
        {
            (void)(ar << ... << std::move(v));
        }, std::move(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Struct T>
    void load(auto& ar, T& v)
    {
        idl::introspection::applyBases([&](auto&... v)
        {
            (void)(ar >> ... >> v);
        }, v);

        idl::introspection::applyFields([&](auto&... v)
        {
            (void)(ar >> ... >> v);
        }, v);
    }
}
