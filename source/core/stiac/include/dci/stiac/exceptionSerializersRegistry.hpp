// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <dci/exception.hpp>
#include <dci/stiac/link/sink.hpp>
#include <dci/stiac/link/source.hpp>

namespace dci::stiac
{
    class API_DCI_STIAC ExceptionSerializersRegistry
    {
    public:
        using Saver = void(*)(link::Sink&, const dci::Exception*);
        using Loader = void(*)(link::Source&, dci::Exception*);

        Saver getSaver(const Eid&);
        Loader getLoader(const Eid&);

    public:
        bool registrate(const char* name, const Eid&, Saver, Loader);
    };

    extern ExceptionSerializersRegistry& exceptionSerializersRegistry API_DCI_STIAC;
}
