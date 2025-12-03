// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include "../oid.hpp"
#include <vector>
#include <string>

namespace dci::aup::instance::setup
{
    API_DCI_AUP void start(const std::vector<std::string>& args);

    API_DCI_AUP bool targetComplete();
    API_DCI_AUP void updateTarget();
    API_DCI_AUP void collectGarbage();
}
