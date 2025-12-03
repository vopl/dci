// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <system_error>
#include <dci/sbs/signal.hpp>

namespace dci::poll
{
    API_DCI_POLL std::error_code    initialize();
    API_DCI_POLL std::error_code    run(bool emitStartedStopped = true);
    API_DCI_POLL sbs::Signal<>      started();
    API_DCI_POLL sbs::Signal<bool>  doSomeWork();
    API_DCI_POLL std::error_code    stop();
    API_DCI_POLL sbs::Signal<>      stopped();
    API_DCI_POLL std::error_code    deinitialize();
}
