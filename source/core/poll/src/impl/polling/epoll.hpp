// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../descriptor.hpp"
#include "../clocking/config.hpp"

namespace dci::poll::impl::polling
{
    class Epoll
    {
    public:
        Epoll();
        ~Epoll();

        std::error_code initialize();

        bool initialized() const;

        std::error_code installDescriptor(Descriptor* d);
        std::error_code uninstallDescriptor(Descriptor* d);

        std::error_code execute(clocking::Duration timeout);
        std::error_code wakeup();

        std::error_code deinitialize();

    private:
        int _fd {-1};
        int _wakeupEvent {-1};
        char _eventsBuffer[8192];
    };
}
