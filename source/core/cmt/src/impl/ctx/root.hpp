// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "config.hpp"

#if defined(DCI_CMT_CONTEXTENGINE_WINFIBER) && DCI_CMT_CONTEXTENGINE_WINFIBER
#   include "engine_winfiber.hpp"
#elif defined(DCI_CMT_CONTEXTENGINE_UCONTEXT) && DCI_CMT_CONTEXTENGINE_UCONTEXT
#   include "engine_ucontext.hpp"
#elif defined(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT) && DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT
#   include "engine_boostcontext.hpp"
#else
#   error "unknown context engine"
#endif

#include <cstdint>

namespace dci::cmt::impl::ctx
{
    class Root
        : public Engine<Root>
    {
        Root& operator=(const Root&) = delete;

    public:
        Root();
        ~Root();

        template <class D2>
        void switchTo(Engine<D2>* to)
        {
            Engine::switchTo(to);
        }
    };
}
