// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <string>
#include <exception>

namespace dci::exception
{
    std::string API_DCI_EXCEPTION toString(std::exception_ptr ptr);
    std::string API_DCI_EXCEPTION currentToString();
}
