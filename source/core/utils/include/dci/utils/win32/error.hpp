// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#ifdef _WIN32
#   include "../api.hpp"
#   include <system_error>
#   include <cstdint>

namespace dci::utils::win32::error
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    enum Value : std::uint32_t {};// DWORD

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_UTILS Category
        : public std::error_category
    {
    public:
        ~Category() override;
        char const* name() const noexcept override final;
        std::string message(int c) const override final;
    };
    API_DCI_UTILS Category& category();
    API_DCI_UTILS std::error_code make_error_code(Value v);
    API_DCI_UTILS std::error_code make(std::uint32_t v);
    API_DCI_UTILS std::error_code last();
}

namespace std
{
    template <>
    struct is_error_code_enum<dci::utils::win32::error::Value> : std::true_type {};
}
#endif
