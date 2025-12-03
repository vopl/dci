// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include "../introspection.hpp"

namespace dci::idl::contract
{
    struct Lid
    {
        uint32 _value {};

        explicit operator bool() const;
        bool operator !() const;

        friend auto operator<=>(const Lid&, const Lid&) = default;
    };

    inline Lid::operator bool() const
    {
        return _value ? true : false;
    }

    inline bool Lid::operator !() const
    {
        return !_value;
    }
}

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<dci::idl::contract::Lid> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<dci::idl::contract::Lid> = 0;
    template <> inline constexpr uint32 fieldsCount<dci::idl::contract::Lid> = 1;
    template <> struct FieldType<dci::idl::contract::Lid, 0> {using result = uint32; };
    template <> inline constexpr auto fieldValue<dci::idl::contract::Lid, 0> = memberValue<dci::idl::contract::Lid, &dci::idl::contract::Lid::_value>;
}
