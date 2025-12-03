// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include "exceptionSerializersRegistry.hpp"
#include <dci/stiac/serialization.hpp>

namespace dci::stiac
{
    template <class E>
    class ExceptionSerializerRegistrator
    {
    public:
        static void saver(link::Sink&, const dci::Exception*);
        static void loader(link::Source&, dci::Exception*);
        static volatile const bool _registrateUtilizer;
    };
}

namespace dci::stiac
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E>
    void ExceptionSerializerRegistrator<E>::saver(link::Sink& sink, const dci::Exception* e)
    {
        sink << *static_cast<const E*>(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E>
    void ExceptionSerializerRegistrator<E>::loader(link::Source& source, dci::Exception* e)
    {
        source >> *static_cast<E*>(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class E>
    volatile const bool ExceptionSerializerRegistrator<E>::_registrateUtilizer = exceptionSerializersRegistry.registrate(
                idl::introspection::typeName<E>.data(),
                E::_eid,
                &ExceptionSerializerRegistrator<E>::saver,
                &ExceptionSerializerRegistrator<E>::loader);
}
