// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"

namespace dci::host::module
{
    struct Entry;

    class API_DCI_HOST StopLocker
    {
    public:
        ~StopLocker();

        StopLocker();
        StopLocker(Entry* e);
        StopLocker(StopLocker&& from);
        StopLocker(const StopLocker& from);

        StopLocker& operator=(StopLocker&& from);
        StopLocker& operator=(const StopLocker& from);

    private:
        Entry* _e {};
    };
}
