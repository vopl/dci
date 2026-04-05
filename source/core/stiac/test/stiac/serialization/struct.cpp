/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/idl/introspection.hpp>

using namespace dci;
using namespace dci::idl;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
struct StructVictimBase
{
    real32 bfld1;
};

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<StructVictimBase> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<StructVictimBase> = 0;
    template <> inline constexpr uint32 fieldsCount<StructVictimBase> = 1;
    template <> struct FieldType<StructVictimBase, 0> {using result = real32; };
    template <> inline constexpr auto fieldValue<StructVictimBase, 0> = memberValue<StructVictimBase, &StructVictimBase::bfld1>;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
struct StructVictim : public StructVictimBase
{
    int fld1;
    std::string fld2;
};

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<StructVictim> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<StructVictim> = 1;
    template <> struct BaseType<StructVictim, 0> {using result = StructVictimBase; };
    template <> inline constexpr uint32 fieldsCount<StructVictim> = 2;
    template <> struct FieldType<StructVictim, 0> {using result = int; };
    template <> struct FieldType<StructVictim, 1> {using result = std::string; };
    template <> inline constexpr auto fieldValue<StructVictim, 0> = memberValue<StructVictim, &StructVictim::fld1>;
    template <> inline constexpr auto fieldValue<StructVictim, 1> = memberValue<StructVictim, &StructVictim::fld2>;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, struct)
{
    hereThere2( StructVictimBase{2.34567f}   ,[&](const auto&, const auto& b)
    {
        EXPECT_FLOAT_EQ(b.bfld1, 2.34567);
    });

    hereThere2( StructVictim{{1.234f}, 42, "42"}   ,[&](const auto& , const auto& b)
    {
        EXPECT_FLOAT_EQ(b.bfld1, 1.234);
        EXPECT_EQ(b.fld1, 42);
        EXPECT_EQ(b.fld2, "42");
    });
}
