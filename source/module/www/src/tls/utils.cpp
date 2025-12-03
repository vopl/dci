// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "utils.hpp"

namespace dci::module::www::tls::utils
{
    std::string errorString(unsigned long error)
    {
        std::string_view libStr{ERR_lib_error_string(error)};
        std::string_view reasonStr{ERR_reason_error_string(error)};

        std::string errorString;
        errorString.reserve(8+4+libStr.size()+reasonStr.size());
        errorString.resize(8);
        errorString.resize(std::to_chars(errorString.data(), errorString.data()+errorString.size(), error, 16).ptr - errorString.data());
        errorString += ':';
        errorString += libStr;
        errorString += ':';
        errorString += reasonStr;
        return errorString;
    }
}
