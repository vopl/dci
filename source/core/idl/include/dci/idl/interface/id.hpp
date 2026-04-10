/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/primitives.hpp>
#include "../api.hpp"
#include "../contract/id.hpp"
#include "../introspection.hpp"
#include "side.hpp"

namespace dci::idl::interface
{
    struct Id
    {
        contract::Id    _cid;
        Side            _side;

        bool API_DCI_IDL fromText(const String& text);
        String API_DCI_IDL toText() const;

        void invert();

        friend auto operator<=>(const Id&, const Id&) = default;
    };

    inline void Id::invert()
    {
        if(Side::primary == _side)  _side = Side::primary;
        else                        _side = Side::opposite;
    }
}

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<dci::idl::interface::Id> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<dci::idl::interface::Id> = 0;
    template <> inline constexpr uint32 fieldsCount<dci::idl::interface::Id> = 2;
    template <> inline constexpr std::array fieldName<dci::idl::interface::Id, 0> = std::to_array("_cid");
    template <> inline constexpr std::array fieldName<dci::idl::interface::Id, 1> = std::to_array("_side");
    template <> struct FieldType<dci::idl::interface::Id, 0> {using result = dci::idl::contract::Id; };
    template <> struct FieldType<dci::idl::interface::Id, 1> {using result = dci::idl::interface::Side; };
    template <> inline constexpr auto fieldValue<dci::idl::interface::Id, 0> = memberValue<dci::idl::interface::Id, &dci::idl::interface::Id::_cid>;
    template <> inline constexpr auto fieldValue<dci::idl::interface::Id, 1> = memberValue<dci::idl::interface::Id, &dci::idl::interface::Id::_side>;
}
