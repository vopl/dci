/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/stiac/interfaceLinksRegistry.hpp>
#include <dci/logger.hpp>

#include <map>

namespace dci::stiac
{
    namespace
    {
        using Key = idl::interface::Lid;

        struct Factories
        {
            InterfaceLinksRegistry::Factory1 _factory1;
            InterfaceLinksRegistry::Factory2 _factory2;
        };

        using Map = std::map<Key, Factories>;

        Map* g_map = nullptr;

        class InterfaceLinksRegistryInstance
            : public InterfaceLinksRegistry
        {
        private:
            link::BasePtr create(const idl::interface::Lid& lid, auto&& factoryCaller)
            {
                if(!g_map)
                {
                    return link::BasePtr();
                }

                auto iter = g_map->find(lid);

                if(g_map->end() == iter)
                {
                    return link::BasePtr();
                }

                return factoryCaller(iter->second);
            }

        public:
            link::BasePtr create1(const idl::interface::Lid& lid, idl::Interface&& interface)
            {
                return create(lid, [&](Factories& factories)
                {
                    return factories._factory1(std::move(interface));
                });
            }

            link::BasePtr create2(const idl::interface::Lid& lid, idl::Interface& interface)
            {
                return create(lid, [&](Factories& factories)
                {
                    return factories._factory2(interface);
                });
            }

            bool registrate(const char* name, const idl::interface::Lid& lid, Factory1 factory1, Factory2 factory2)
            {
                if(!g_map)
                {
                    g_map = new Map;
                }

                auto res = g_map->emplace(lid, Factories{factory1, factory2});
                if(!res.second)
                {
                    LOGW("stiac link factory already registred: "<<name);
                }
                else
                {
                    //LOGI("stiac link factory registred: "<<name);
                }

                volatile bool result {true};
                return result;
            }
        } interfaceLinksRegistryInstance;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::BasePtr InterfaceLinksRegistry::create1(const idl::interface::Lid& lid, idl::Interface&& interface)
    {
        return static_cast<InterfaceLinksRegistryInstance*>(this)->create1(lid, std::move(interface));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::BasePtr InterfaceLinksRegistry::create2(const idl::interface::Lid& lid, idl::Interface& interface)
    {
        return static_cast<InterfaceLinksRegistryInstance*>(this)->create2(lid, interface);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool InterfaceLinksRegistry::registrate(const char* name, const idl::interface::Lid& lid, Factory1 factory1, Factory2 factory2)
    {
        return static_cast<InterfaceLinksRegistryInstance*>(this)->registrate(name, lid, factory1, factory2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    InterfaceLinksRegistry& interfaceLinksRegistry API_DCI_STIAC = interfaceLinksRegistryInstance;
}
