// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include "link/base.hpp"
#include <dci/idl/interface.hpp>
#include <dci/idl/interface/lid.hpp>

namespace dci::stiac
{
    class API_DCI_STIAC InterfaceLinksRegistry
    {
    public:
        using Factory1 = link::BasePtr(*)(idl::Interface&& interface);
        using Factory2 = link::BasePtr(*)(idl::Interface& interface);

        link::BasePtr create1(const idl::interface::Lid& lid, idl::Interface&& interface);
        link::BasePtr create2(const idl::interface::Lid& lid, idl::Interface& interface);

    public:
        bool registrate(const char* name, const idl::interface::Lid& lid, Factory1 factory1, Factory2 factory2);
    };

    extern InterfaceLinksRegistry& interfaceLinksRegistry API_DCI_STIAC;
}
