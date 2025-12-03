// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "config/api.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>
#include <chrono>

namespace dci::idl::gen
{
    struct Config;
}

namespace dci::config
{
    using ptree = boost::property_tree::ptree;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    dci::idl::gen::Config API_DCI_CONFIG cnvt(const ptree& pt);
    ptree API_DCI_CONFIG cnvt(const dci::idl::gen::Config& c);
    ptree API_DCI_CONFIG parse(const std::vector<std::string>& argv);

    boost::optional<bool> API_DCI_CONFIG parseBool(const std::string_view& str);
    boost::optional<std::chrono::nanoseconds> API_DCI_CONFIG parseTime(const std::string_view& str);
    std::string API_DCI_CONFIG generateTime(std::chrono::nanoseconds v);
}

namespace boost::property_tree
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <typename Ch, typename Traits, typename Alloc>
    struct translator_between<std::basic_string<Ch, Traits, Alloc>, bool>
    {
        using string = std::basic_string<Ch, Traits, Alloc>;
        using target = bool;
        struct type
        {
            boost::optional<target> get_value(const string& v)
            {
                return ::dci::config::parseBool(v);
            }
            boost::optional<string> put_value(const target& v)
            {
                return boost::optional<string>{v ? "true" : "false" };
            }
        };
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <typename Ch, typename Traits, typename Alloc, typename Rep, typename Period>
    struct translator_between<std::basic_string<Ch, Traits, Alloc>, std::chrono::duration<Rep, Period>>
    {
        using string = std::basic_string<Ch, Traits, Alloc>;
        using target = std::chrono::duration<Rep, Period>;
        struct type
        {
            boost::optional<target> get_value(const string& v)
            {
                return ::dci::config::parseTime(v).map([](std::chrono::nanoseconds v){return std::chrono::duration_cast<target>(v);});
            }
            boost::optional<string> put_value(const target& v)
            {
                return ::dci::config::generateTime(std::chrono::duration_cast<std::chrono::nanoseconds>(v));
            }
        };
    };
}
