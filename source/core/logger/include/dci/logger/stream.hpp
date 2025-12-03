// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <sstream>
#include <string_view>
#include <system_error>

namespace dci::logger
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_LOGGER Stream
    {
    public:
        Stream(std::string_view level, std::string_view identity);
        ~Stream();

        Stream& operator<<(const std::error_code& ec);
        Stream& operator<<(const std::error_condition& ec);
        template <class T> Stream& operator<<(const T& v);

    private:
        std::stringstream _buf;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> Stream& Stream::operator<<(const T& v)
    {
        _buf << v;
        return *this;
    }
}
