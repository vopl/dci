// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
