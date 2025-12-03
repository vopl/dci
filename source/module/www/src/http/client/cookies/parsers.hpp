// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "www.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace dci::module::www::http::client::cookies::parsers
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Id = unsigned>
    struct str2id : x3::parser<str2id<Id>>
    {
        typedef Id attribute_type;
        static bool const has_attribute = !std::is_same<x3::unused_type, attribute_type>::value;
        static bool const handles_container = has_attribute;

        constexpr str2id(std::string_view str, Id id)
          : str{str}
          , id{id}
        {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, x3::unused_type, Attribute_& attr) const
        {
            x3::skip_over(first, last, context);
            x3::unused_type unused{};
            bool res = x3::detail::string_parse(str, first, last, unused, [](char a, char b)
            {
                if(a >= 'A' && a <= 'Z') a = a - 'A' + 'a';
                if(b >= 'A' && b <= 'Z') b = b - 'A' + 'a';
                return int(a) - int(b);
            });
            if(res)
                attr = id;
            return res;
        }

        std::string_view str;
        Id id;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr auto sp = x3::lit(' ');
    constexpr auto ctl = x3::omit[x3::char_('\x0', '\x1f') | x3::char_('\x7e')];
    constexpr auto separator = x3::omit[x3::char_('(') | ')' | '<' | '>' | '@'
                               | ',' | ';' | ':' | '\\' | '\''
                               | '/' | '[' | ']' | '?' | '='
                               | '{' | '}' | ' ' | '\t'];

    constexpr auto token = x3::omit[+(x3::char_ - ctl - separator)];
    constexpr auto cookieOctet = x3::omit[x3::char_('\x21', '\x3A') | x3::char_('\x3C', '\x7E')];// тут отступление от спецификации, некоторые сайты позволяют себе всякое, выножденно следуем за ними

    constexpr auto dquote = x3::lit('"');

    constexpr auto wkday = x3::lit("Mon") | "Tue" | "Wed" | "Thu" | "Fri" | "Sat" | "Sun";
    // constexpr auto weekday = x3::lit("Monday") | "Tuesday" | "Wednesday" | "Thursday" | "Friday" | "Saturday" | "Sunday";
    //constexpr auto month = x3::lit("Jan") | "Feb" | "Mar" | "Apr" | "May" | "Jun" | "Jul" | "Aug" | "Sep" | "Oct" | "Nov" | "Dec";
    constexpr auto month =
            str2id("Jan", std::chrono::January) |
            str2id("Feb", std::chrono::February) |
            str2id("Mar", std::chrono::March) |
            str2id("Apr", std::chrono::April) |
            str2id("May", std::chrono::May) |
            str2id("Jun", std::chrono::June) |
            str2id("Jul", std::chrono::July) |
            str2id("Aug", std::chrono::August) |
            str2id("Sep", std::chrono::September) |
            str2id("Oct", std::chrono::October) |
            str2id("Nov", std::chrono::November) |
            str2id("Dec", std::chrono::December);

    constexpr auto twoDigit = x3::uint_parser<unsigned, 10, 2, 2>{};
    //constexpr auto fourDigit = x3::uint_parser<unsigned, 10, 4, 4>{};
    constexpr auto twoOrFourDigit = x3::uint_parser<unsigned, 10, 2, 4>{};
    constexpr auto time = twoDigit >> ':' >> twoDigit >> ':' >> twoDigit;

    // constexpr auto date3 = month >> sp >>  ( twoDigit | ( sp >> x3::digit ));
    // constexpr auto date2 = twoDigit >> '-' >> month >> '-' >> twoDigit;
    constexpr auto date1 = twoDigit >> (sp|'-') >> month >> (sp|'-') >> twoOrFourDigit;

    // constexpr auto asctimeDate = wkday >> sp >> date3 >> sp >> time >> sp >> fourDigit;
    // constexpr auto rfc850Date  = weekday >> ',' >> sp >> date2 >> sp >> time >> sp >> "GMT";
    constexpr auto rfc1123Date = wkday >> ',' >> sp >> date1 >> sp >> time >> sp >> "GMT";

    // constexpr auto httpDate = rfc1123Date | rfc850Date | asctimeDate;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr auto cookieName = x3::raw[token][([](auto& ctx)
    {
        auto irange = x3::_attr(ctx);
        x3::get<api::http::client::cookies::Entry>(ctx).id.name.assign(irange.begin(), irange.end());
    })];

    constexpr auto cookieValue = x3::raw[*cookieOctet][([](auto& ctx)
    {
        auto irange = x3::_attr(ctx);
        x3::get<api::http::client::cookies::Entry>(ctx).value.assign(irange.begin(), irange.end());
    })];

    constexpr auto cookiePair = cookieName >> '=' >> cookieValue;

    constexpr auto saneCookieDate = rfc1123Date;

    constexpr auto expiresAV = x3::no_case[x3::lit("Expires=")] >> saneCookieDate[([](auto& ctx)
    {
        auto attr = x3::_attr(ctx);

        auto D = boost::fusion::at_c<0>(attr);
        auto M = boost::get<std::chrono::month>(boost::fusion::at_c<1>(attr));
        auto Y = boost::fusion::at_c<2>(attr);
        if(Y < 100)
            Y += 2000;
        auto h = boost::fusion::at_c<3>(attr);
        auto m = boost::fusion::at_c<4>(attr);
        auto s = boost::fusion::at_c<5>(attr);

        std::chrono::year_month_day ymd{std::chrono::year{(int)Y}, M, std::chrono::day{D}};
        if(!ymd.ok())        return;
        if(h < 0 || h >= 24) return;
        if(m < 0 || h >= 60) return;
        if(s < 0 || h >= 60) return;

        auto tp = std::chrono::sys_days{ymd} + std::chrono::hours{h} + std::chrono::minutes{m} + std::chrono::seconds{s};
        api::http::client::cookies::Entry& entry = x3::get<api::http::client::cookies::Entry>(ctx);
        entry.expiryTime = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        entry.persistent = true;
    })];

    constexpr auto maxAgeAV = x3::no_case[x3::lit("Max-Age=")] >> x3::int_parser<int64>{}[([](auto& ctx)
    {
        int64 seconds{x3::_attr(ctx)};
        api::http::client::cookies::Entry& entry = x3::get<api::http::client::cookies::Entry>(ctx);
        if(seconds <= 0)
        {
            entry.expiryTime = {};
            return;
        }
        entry.expiryTime = entry.creationTime + seconds;
        entry.persistent = true;
    })];
    constexpr auto domainAV = x3::no_case[x3::lit("Domain=")] >> x3::raw[*(x3::char_ - ctl - ';')][([](auto& ctx)
    {
        auto irange = x3::_attr(ctx);
        x3::get<api::http::client::cookies::Entry>(ctx).id.domain.assign(irange.begin(), irange.end());
    })];
    constexpr auto pathAV = x3::no_case[x3::lit("Path=")] >> x3::raw[*(x3::char_ - ctl - ';')][([](auto& ctx)
    {
        auto irange = x3::_attr(ctx);
        x3::get<api::http::client::cookies::Entry>(ctx).id.path.assign(irange.begin(), irange.end());
    })];
    constexpr auto sameSiteAV = x3::no_case[x3::lit("SameSite=")] >> (str2id("Strict", api::http::client::cookies::SameSite::strict)|str2id("Lax", api::http::client::cookies::SameSite::lax)|str2id("None", api::http::client::cookies::SameSite::none))[([](auto& ctx)
    {
        x3::get<api::http::client::cookies::Entry>(ctx).sameSite = boost::get<api::http::client::cookies::SameSite>(x3::_attr(ctx));
    })];
    constexpr auto secureAV = x3::no_case[x3::lit("Secure")][([](auto& ctx)
    {
        x3::get<api::http::client::cookies::Entry>(ctx).secure = true;
    })];
    constexpr auto httpOnlyAV = x3::no_case[x3::lit("HttpOnly")][([](auto& ctx)
    {
        x3::get<api::http::client::cookies::Entry>(ctx).httpOnly = true;
    })];
    constexpr auto partitionedAV = x3::no_case[x3::lit("Partitioned")][([](auto& ctx)
    {
        x3::get<api::http::client::cookies::Entry>(ctx).partitioned = true;
    })];
    constexpr auto extensionAV = x3::raw[*(x3::char_ - ctl - ';')][([](auto& ctx)
    {
        auto irange = x3::_attr(ctx);
        LOGD("extensionAV: [" << std::string_view{irange.begin(), irange.end()} << "]");
    })];

    constexpr auto cookieAV = expiresAV | maxAgeAV |
                              domainAV | pathAV |
                              sameSiteAV |
                              secureAV | httpOnlyAV | partitionedAV |
                              extensionAV;

    constexpr auto setCookieString = cookiePair >> *( x3::lit(';') >> -sp >> cookieAV );
}
