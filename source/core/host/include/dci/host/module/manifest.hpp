/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../api.hpp"
#include <dci/idl/iId.hpp>
#include <dci/idl/iSide.hpp>
#include <dci/idl/contract/mdDescriptor.hpp>
#include <dci/idl/introspection.hpp>
#include <algorithm>
#include <string>
#include <vector>

namespace dci::host::module
{
    struct API_DCI_HOST Manifest
    {
        bool        _valid = false;

        std::string _name;
        std::string _mainBinary;

        struct ServiceId
        {
            idl::IId    _iid {};
            std::string _alias;

            ServiceId()
            {
            }

            ServiceId(const idl::IId& iid)
                : _iid(iid)
            {
            }

            ServiceId(const idl::IId& iid, const std::string& alias)
                : _iid(iid)
                , _alias(alias)
            {
            }

            ServiceId(const idl::IId& iid, std::string&& alias)
                : _iid(iid)
                , _alias(std::move(alias))
            {
            }
        };

        template <template <idl::ISide> class C, idl::ISide s = idl::ISide::primary>
        void pushServiceId()
        {
            auto iname = idl::introspection::typeName<C<s>>;
            std::string cname{iname.data(), iname.size()-1};

            static constexpr std::string_view prefix{ "dci::idl::gen::" };
            if(cname.starts_with(prefix))
            {
                cname.erase(cname.begin(), cname.begin()+prefix.size());//откусываем не нужный префикс
            }

            static_assert(s == idl::ISide::primary);
            cname.erase(std::find(cname.begin(), cname.end(), '<'), cname.end());//откусываем специализацию стороной оставляя только имя контракта

            _serviceIds.emplace_back(C<s>::Internal::id(), std::move(cname));
        }

        std::vector<ServiceId>   _serviceIds;

        void reset();

        bool fromConf(const std::string& conf);
        bool fromConfFile(const std::string& path);
        std::string toConf() const;
    };
}
