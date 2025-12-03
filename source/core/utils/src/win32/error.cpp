// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/utils/win32/error.hpp>
#include <windows.h>

namespace dci::utils::win32::error
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Category::~Category()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    char const* Category::name() const noexcept
    {
        return "win32 error";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Category::message(int c) const
    {
        char buffer[256];
        auto len = FormatMessage(
                       FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL,
                       static_cast<DWORD>(c),
                       0,
                       buffer,
                       sizeof(buffer),
                       NULL);
        if(!len)
        {
            return "unknown";
        }

        while(len && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
        {
            --len;
        }

        return std::string{buffer, len};
    }

    namespace
    {
        Category g_category;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Category& category()
    {
        return g_category;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code make_error_code(Value v)
    {
        return std::error_code{static_cast<int>(v), category()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code make(std::uint32_t v)
    {
        return make_error_code(static_cast<error::Value>(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code last()
    {
        return make(GetLastError());
    }
}

