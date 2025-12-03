// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>
#include <dci/idl/introspection.hpp>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept Exception = std::is_base_of_v<dci::Exception, T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Exception T>
    void save(auto& ar, const T& v)
    {
        ar << v.whatBuffer();

        if constexpr(idl::introspection::isException<T>)
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
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Exception T>
    void save(auto& ar, T&& v)
    {
        ar << std::move(v.whatBuffer());

        if constexpr(idl::introspection::isException<T>)
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
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Exception T>
    void load(auto& ar, T& v)
    {
        ar >> v.whatBuffer();

        if constexpr(idl::introspection::isException<T>)
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
}
