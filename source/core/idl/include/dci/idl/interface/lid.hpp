// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include "../contract/lid.hpp"
#include "side.hpp"

namespace dci::idl::interface
{
    struct Lid
    {
        contract::Lid   _clid;
        Side            _side;

        bool API_DCI_IDL fromIidText(const String& text);
        String API_DCI_IDL toIidText() const;

        explicit operator bool() const;
        bool operator !() const;

        void invert();

        friend auto operator<=>(const Lid&, const Lid&) = default;
    };

    inline Lid::operator bool() const
    {
        return _clid.operator bool();
    }

    inline bool Lid::operator !() const
    {
        return !_clid;
    }

    inline void Lid::invert()
    {
        if(Side::primary == _side)  _side = Side::opposite;
        else                        _side = Side::primary;
    }
}

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<dci::idl::interface::Lid> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<dci::idl::interface::Lid> = 0;
    template <> inline constexpr uint32 fieldsCount<dci::idl::interface::Lid> = 2;
    template <> struct FieldType<dci::idl::interface::Lid, 0> {using result = dci::idl::contract::Lid; };
    template <> struct FieldType<dci::idl::interface::Lid, 1> {using result = dci::idl::interface::Side; };
    template <> inline constexpr auto fieldValue<dci::idl::interface::Lid, 0> = memberValue<dci::idl::interface::Lid, &dci::idl::interface::Lid::_clid>;
    template <> inline constexpr auto fieldValue<dci::idl::interface::Lid, 1> = memberValue<dci::idl::interface::Lid, &dci::idl::interface::Lid::_side>;
}
