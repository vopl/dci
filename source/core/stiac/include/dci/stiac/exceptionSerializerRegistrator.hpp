/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
