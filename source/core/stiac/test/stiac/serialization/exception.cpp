/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/stiac.hpp>
#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/exception.hpp>
#include <dci/exception/toString.hpp>

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, exception)
{
    hereThere2( std::make_exception_ptr(int())                       ,[&](const auto&, const auto& b){EXPECT_EQ(std::string("unknown exception"), dci::exception::toString(b));});
    hereThere2( std::make_exception_ptr(std::runtime_error("xyz"))   ,[&](const auto&, const auto& b){EXPECT_EQ(std::string("runtime_error{xyz}"), dci::exception::toString(b));});
    hereThere2( std::make_exception_ptr(dci::Exception("dci"))       ,[&](const auto&, const auto& b){EXPECT_EQ(std::string("dci::Exception{dci}"), dci::exception::toString(b));});

    try
    {
        try
        {
            try
            {
                try
                {
                    throw 220;
                }
                catch(...)
                {
                    std::throw_with_nested(std::runtime_error("level0"));
                }
            }
            catch(...)
            {
                std::throw_with_nested(std::logic_error("level1"));
            }
        }
        catch(...)
        {
            std::throw_with_nested(dci::Exception("level2"));
        }
    }
    catch(...)
    {
        hereThere2( std::current_exception()   ,[&](const auto&, const auto& b){EXPECT_EQ(std::string("dci::Exception{level2}, caused by logic_error{level1}, caused by runtime_error{level0}, caused by unknown exception"), dci::exception::toString(b));});
    }
}
