// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include "../oid.hpp"
#include "object.hpp"
#include <dci/bytes.hpp>

namespace dci::aup::catalog
{
    Oid API_DCI_AUP identify(const Bytes& blob);
    Oid API_DCI_AUP identify(std::FILE* f);
    Oid API_DCI_AUP identify(const catalog::Object* object);
}
