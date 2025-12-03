// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/config.hpp>
#include <dci/logger.hpp>
#include <regex>
#include <format>
#include <spanstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <boost/spirit/home/x3.hpp>

#include "idl-config.hpp"


namespace dci::config
{
    using namespace boost::property_tree;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    dci::idl::gen::Config cnvt(const ptree& pt)
    {
        dci::idl::gen::Config res;
        res.value = pt.data();

        for(const auto& kv : pt)
        {
            res.children.emplace_back(std::make_tuple(kv.first, cnvt(kv.second)));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ptree cnvt(const dci::idl::gen::Config& c)
    {
        ptree res;
        res.data() = c.value;

        for(const auto& kv : c.children)
        {
            res.push_back(std::make_pair(std::get<0>(kv), cnvt(std::get<1>(kv))));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        bool ends_with(const std::string& str, const char* suffix)
        {
            size_t slen = strlen(suffix);

            if(str.size() < slen)
            {
                return false;
            }

            return str.size() - slen == str.find(suffix);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ptree parse(const std::vector<std::string>& argv)
    {
        ptree pt;

        for(const std::string& arg : argv)
        {
            if(0 == arg.find('@'))
            {
                if(ends_with(arg, ".xml"))
                {
                    read_xml(arg.substr(1), pt);
                }
                else if(ends_with(arg, ".ini"))
                {
                    read_ini(arg.substr(1), pt);
                }
                else if(ends_with(arg, ".json"))
                {
                    read_json(arg.substr(1), pt);
                }
                else
                {
                    read_info(arg.substr(1), pt);
                }
                continue;
            }

            std::string::size_type eqPos = arg.find('=');

            if(std::string::npos == eqPos)
            {
                pt.put(arg.substr(0, eqPos), std::string{});
            }
            else
            {
                pt.put(arg.substr(0, eqPos), arg.substr(eqPos+1));
            }
        }

        return pt;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    boost::optional<bool> parseBool(const std::string_view& str)
    {
        if(str.empty())
            return {};

        static const std::regex t("^(t|true|on|enable|allow|1)$", std::regex_constants::icase | std::regex::optimize);
        static const std::regex f("^(f|false|off|disable|deny|0)$", std::regex_constants::icase | std::regex::optimize);

        if(std::regex_match(str.begin(), str.end(), t))
            return {true};
        if(std::regex_match(str.begin(), str.end(), f))
            return {false};

        LOGW("unable to parse boolean value: `" << str << "`");
        return boost::none;
    }

    namespace parseTimeSpares
    {
        using namespace boost::spirit;

        constexpr auto i0 = x3::uint64[([](auto& ctx){x3::_val(ctx).integer = /*x3::_val(ctx).integer*60 +*/ x3::_attr(ctx);})];
        constexpr auto i1 = x3::uint64[([](auto& ctx){x3::_val(ctx).integer =   x3::_val(ctx).integer*60 +   x3::_attr(ctx);})];

        constexpr auto parser_def
        {
            i0 >> -(':' >> i1 >> -(':' >> i1)) >>
            -(
                x3::char_('.') >>
                -(
                    x3::eps   [([](auto& ctx){x3::_val(ctx).fractionalWidth  = x3::_where(ctx).end()-x3::_where(ctx).begin();})] >>
                    x3::uint64[([](auto& ctx){x3::_val(ctx).fractional = x3::_attr(ctx);})]
                 )
             ) >>
            x3::eoi
        };

        struct Data
        {
            uint64      integer         {};
            std::size_t fractionalWidth {};
            uint64      fractional      {};
        };

        constexpr x3::rule<Data, Data> parser{""};

        BOOST_SPIRIT_DEFINE(parser);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    boost::optional<std::chrono::nanoseconds> API_DCI_CONFIG parseTime(const std::string_view& str)
    {
        using namespace parseTimeSpares;

        Data data{};
        if(!x3::parse(str.begin(), str.end(), parser, data))
        {
            LOGW("unable to parse time value: `" << str << "`");
            return boost::none;
        }

        using ratio = std::nano;
        int64 value = data.integer * ratio::den / ratio::num;

        if(data.fractional)
        {
            constexpr int64 order = []
            {
                int64 res = 0;
                for(auto v{ratio::den}; v>1; v/=10) ++res;
                for(auto v{ratio::num}; v>1; v/=10) --res;
                return res;
            }();
            static_assert(0 < order);

            std::size_t width = data.fractionalWidth;
            for(; width>order; --width) data.fractional /= 10;
            for(; width<order; ++width) data.fractional *= 10;

            value += data.fractional;
        }

        return std::chrono::duration<int64, ratio>{value};

        // std::chrono::microseconds us;
        // if( (std::ispanstream{str} >> parse("%1H:%1M:%1S", us)) ||
        //     (std::ispanstream{str} >> parse("%1M:%1S", us)) ||
        //     (std::ispanstream{str} >> parse("%1S", us)))
        // {
        //     return std::chrono::duration_cast<std::chrono::nanoseconds>(us);
        // }

        // LOGW("unable to parse time value: `" << str << "`");
        // return boost::none;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string API_DCI_CONFIG generateTime(std::chrono::nanoseconds ns)
    {
        std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(ns);
        return std::format("{:%T}", us);
    }
}
