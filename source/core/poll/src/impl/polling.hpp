// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "descriptor.hpp"

#include <dci/utils/intrusiveDlist.hpp>

#ifdef _WIN32
#   include "polling/asyncSelect.hpp"
#else
#   include "polling/epoll.hpp"
#endif

#include <system_error>
#include <chrono>

namespace dci::poll::impl
{
    class Polling
    {
    public:
        Polling();
        ~Polling();

        std::error_code initialize();

        bool initialized() const;

        std::error_code installDescriptor(Descriptor* d);
        std::error_code uninstallDescriptor(Descriptor* d);

        std::error_code execute(clocking::Duration timeout);
        std::error_code wakeup();

        std::error_code deinitialize();

    public:
        bool hasPayload() const;

    private:
        utils::IntrusiveDlist<Descriptor> _descriptors;

#ifdef _WIN32
        polling::AsyncSelect    _engine;
#else
        polling::Epoll          _engine;
#endif
    };
}
