// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include "../api.hpp"
#include "../contract/id.hpp"
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
