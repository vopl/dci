/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/stiac/exceptionSerializersRegistry.hpp>
#include <dci/logger.hpp>

#include <map>

namespace dci::stiac
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        ExceptionSerializersRegistry g_exceptionSerializersRegistryInstance{};
        std::map<Eid, std::pair<ExceptionSerializersRegistry::Saver, ExceptionSerializersRegistry::Loader>>* g_map = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionSerializersRegistry::Saver ExceptionSerializersRegistry::getSaver(const Eid& eid)
    {
        if(!g_map)
        {
            return {};
        }

        auto iter = g_map->find(eid);
        dbgAssert(iter != g_map->end());
        if(iter == g_map->end())
        {
            return {};
        }

        return iter->second.first;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionSerializersRegistry::Loader ExceptionSerializersRegistry::getLoader(const Eid& eid)
    {
        if(!g_map)
        {
            return {};
        }

        auto iter = g_map->find(eid);
        dbgAssert(iter != g_map->end());
        if(iter == g_map->end())
        {
            return {};
        }

        return iter->second.second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool ExceptionSerializersRegistry::registrate(const char* /*name*/, const Eid& eid, Saver saver, Loader loader)
    {
        if(!g_map)
        {
            g_map = new std::map<Eid, std::pair<ExceptionSerializersRegistry::Saver, ExceptionSerializersRegistry::Loader>>;
        }

        auto res = g_map->emplace(eid, std::pair{saver, loader});
        if(!res.second)
        {
            // такое вполне может быть, разные модули регистрируют и не знают что другие делают то же самое
            //LOGW("exception serializers already registred: "<<name);
        }

        volatile bool result {true};
        return result;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionSerializersRegistry& exceptionSerializersRegistry API_DCI_STIAC = g_exceptionSerializersRegistryInstance;
}

#include <dci/stiac/exceptionSerializerRegistrator.hpp>
#include <dci/exception.hpp>
template class dci::stiac::ExceptionSerializerRegistrator<dci::Exception>;
