// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/eid.hpp>
#include <map>
#include <string>
#include <exception>

namespace dci::exception::registry
{
    using Factory = std::exception_ptr(*)(const std::exception_ptr& cause);

    struct Entry
    {
        Factory     _factory;
        std::string _sign;
    };

    using Entries = std::map<Eid, Entry>;

    Entries& map();
}
