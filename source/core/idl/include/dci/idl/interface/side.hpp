// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include "../introspection.hpp"

namespace dci::idl::interface
{
    enum class Side : uint8
    {
        primary     = 1,
        opposite    = 2,
    };

    constexpr Side invert(Side s)
    {
        switch(s)
        {
        default:
        case Side::primary:  return Side::opposite;
        case Side::opposite: return Side::primary;
        }
    }
}

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<dci::idl::interface::Side> = Kind::enum_;
    template <> inline constexpr uint32 fieldsCount<dci::idl::interface::Side> = 2;
    template <> struct FieldType<dci::idl::interface::Side, 0> {using result = dci::idl::interface::Side; };
    template <> struct FieldType<dci::idl::interface::Side, 1> {using result = dci::idl::interface::Side; };
    template <> inline constexpr auto fieldValue<dci::idl::interface::Side, 0> = dci::idl::interface::Side::primary;
    template <> inline constexpr auto fieldValue<dci::idl::interface::Side, 1> = dci::idl::interface::Side::opposite;
}
