// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <system_error>

namespace dci::poll
{
    enum class error
    {
        unknown = 1,
        already_initialized,
        already_started,
        already_stopped,
        no_engine_available,
        not_initialized,
        not_stopped,
        bad_descriptor,
        already_installed,
        not_installed,
    };

    API_DCI_POLL const std::error_category& error_category();
    API_DCI_POLL std::error_code make_error_code(error e);
}

namespace std
{
    template<>
    struct is_error_code_enum<dci::poll::error> : public true_type {};
}
