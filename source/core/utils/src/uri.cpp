// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>

namespace boost::spirit::x3::traits
{
    template <typename It>
    void move_to(It b, It e, std::string_view& dest)
    {
        dest = std::string_view(b, e);
    }

    template <typename It>
    void move_to(It b, It e, std::optional<std::string_view>& dest)
    {
        dest = std::string_view(b, e);
    }

    template <typename It>
    void move_to(It b, It e, std::string& dest)
    {
        dest = std::string(b, e);
    }

    template <typename It>
    void move_to(It b, It e, std::optional<std::string>& dest)
    {
        dest = std::string(b, e);
    }

    template <typename It, class String>
    void move_to(It b, It e, dci::utils::uri::networkNode::RegName<String>& dest)
    {
        move_to(b, e, static_cast<String&>(dest));
    }

    template <typename It, class String>
    void move_to(It b, It e, dci::utils::uri::networkNode::Ip4<String>& dest)
    {
        move_to(b, e, static_cast<String&>(dest));
    }

    template <typename It, class String>
    void move_to(It b, It e, dci::utils::uri::networkNode::Ip6<String>& dest)
    {
        move_to(b, e, static_cast<String&>(dest));
    }

    template <typename It, class String>
    void move_to(It b, It e, dci::utils::uri::networkNode::IpFuture<String>& dest)
    {
        move_to(b, e, static_cast<String&>(dest));
    }
}

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

namespace boost::spirit::x3
{
    template <class Var, class Alternative>
    struct transform_attribute<Var, Alternative, std::void_t<decltype(Var{} = Alternative{})>>
    {
        typedef Alternative type;
        static Alternative pre(Var&)
        {
            return Alternative{};
        }
        static void post(Var& var, Alternative&& alt)
        {
            var = std::forward<Alternative>(alt);
        }
    };
}

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (String),
    (dci::utils::uri::www::Userinfo) (String),
    (String, _name),
    (std::optional<String>, _password)
);

namespace x3 = boost::spirit::x3;
using namespace std::string_view_literals;
using namespace dci::utils;
using namespace dci::utils::uri;

namespace
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const auto alpha = x3::char_('a', 'z') | x3::char_('A', 'Z');
    const auto digit = x3::char_('0', '9');
    const auto hexDig = x3::char_('0', '9') | x3::char_('a', 'f') | x3::char_('A', 'F');

    // unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
    const auto unreserved = alpha | digit | x3::char_("-._~");
    // pct-encoded = "%" HEXDIG HEXDIG
    const auto pctEncoded = x3::char_('%') >> hexDig >> hexDig;
    // sub-delims  = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
    const auto subDelims  = x3::char_("!$&'()*+,;=");

    // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    const auto pchar = unreserved | pctEncoded | subDelims | ':' | '@';

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
    const auto username = *(unreserved | pctEncoded | subDelims);
    const auto userpasswd = *(unreserved | pctEncoded | subDelims | ':');

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // dec-octet   = DIGIT                 ; 0-9
    //             / %x31-39 DIGIT         ; 10-99
    //             / "1" 2DIGIT            ; 100-199
    //             / "2" %x30-34 DIGIT     ; 200-249
    //             / "25" %x30-35          ; 250-255
    const auto decOctet =
          ("25" >> x3::char_('0', '5'))
        | ('2' >> x3::char_('0', '4') >> digit)
        | ('1' >> digit >> digit)
        | (x3::char_('1', '9') >> digit)
        | digit;

    // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
    const auto ip4 = decOctet >> '.' >> decOctet >> '.' >> decOctet >> '.' >> decOctet;

    // h16         = 1*4HEXDIG
    //             ; 16 bits of address represented in hexadecimal
    const auto h16 = x3::repeat(1, 4)[hexDig];

    // ls32        = ( h16 ":" h16 ) / IPv4address
    //             ; least-significant 32 bits of address
    const auto ls32 = ( h16 >> ':' >> h16 ) | ip4;

    // IPv6address =                            6( h16 ":" ) ls32
    //             /                       "::" 5( h16 ":" ) ls32
    //             / [               h16 ] "::" 4( h16 ":" ) ls32
    //             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
    //             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
    //             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
    //             / [ *4( h16 ":" ) h16 ] "::"              ls32
    //             / [ *5( h16 ":" ) h16 ] "::"              h16
    //             / [ *6( h16 ":" ) h16 ] "::"
    const auto ip6 =
        (
              (                                               x3::repeat(6)[h16 >> ':'] >> ls32)

            | (                                       "::" >> x3::repeat(5)[h16 >> ':'] >> ls32)

            | (-(                             h16) >> "::" >> x3::repeat(4)[h16 >> ':'] >> ls32)

            | (-(                             h16) >> "::" >> x3::repeat(3)[h16 >> ':'] >> ls32)
            | (-(              h16 >> ':'  >> h16) >> "::" >> x3::repeat(3)[h16 >> ':'] >> ls32)

            | (-(                             h16) >> "::" >> x3::repeat(2)[h16 >> ':'] >> ls32)
            | (-(              h16 >> ':'  >> h16) >> "::" >> x3::repeat(2)[h16 >> ':'] >> ls32)
            | (-(x3::repeat(2)[h16 >> ':'] >> h16) >> "::" >> x3::repeat(2)[h16 >> ':'] >> ls32)

            | (-(                             h16) >> "::" >>               h16 >> ':'  >> ls32)
            | (-(              h16 >> ':'  >> h16) >> "::" >>               h16 >> ':'  >> ls32)
            | (-(x3::repeat(2)[h16 >> ':'] >> h16) >> "::" >>               h16 >> ':'  >> ls32)
            | (-(x3::repeat(3)[h16 >> ':'] >> h16) >> "::" >>               h16 >> ':'  >> ls32)

            | (-(                             h16) >> "::" >>                              ls32)
            | (-(              h16 >> ':'  >> h16) >> "::" >>                              ls32)
            | (-(x3::repeat(2)[h16 >> ':'] >> h16) >> "::" >>                              ls32)
            | (-(x3::repeat(3)[h16 >> ':'] >> h16) >> "::" >>                              ls32)
            | (-(x3::repeat(4)[h16 >> ':'] >> h16) >> "::" >>                              ls32)

            | (-(                             h16) >> "::" >>                              h16 )
            | (-(              h16 >> ':'  >> h16) >> "::" >>                              h16 )
            | (-(x3::repeat(2)[h16 >> ':'] >> h16) >> "::" >>                              h16 )
            | (-(x3::repeat(3)[h16 >> ':'] >> h16) >> "::" >>                              h16 )
            | (-(x3::repeat(4)[h16 >> ':'] >> h16) >> "::" >>                              h16 )
            | (-(x3::repeat(5)[h16 >> ':'] >> h16) >> "::" >>                              h16 )

            | (-(                             h16) >> "::"                                     )
            | (-(              h16 >> ':'  >> h16) >> "::"                                     )
            | (-(x3::repeat(2)[h16 >> ':'] >> h16) >> "::"                                     )
            | (-(x3::repeat(3)[h16 >> ':'] >> h16) >> "::"                                     )
            | (-(x3::repeat(4)[h16 >> ':'] >> h16) >> "::"                                     )
            | (-(x3::repeat(5)[h16 >> ':'] >> h16) >> "::"                                     )
            | (-(x3::repeat(6)[h16 >> ':'] >> h16) >> "::"                                     )
        ) >>
        -('%' >> *pchar); // и это отступление от спеки

    // IPvFuture  = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    const auto ipFuture = 'v' >> +hexDig >> '.' >> +(unreserved | subDelims | ':') >> -('%' >> *pchar);

    // IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
    const auto ipLiteral = '[' >> (ip6 | ipFuture) >> ']';

    // reg-name    = *( unreserved / pct-encoded / sub-delims )
    const auto regName = +(unreserved | pctEncoded | subDelims);// тут отклонение от спецификации, дополнительно потребуем непустоту

    // host        = IP-literal / IPv4address / reg-name
    const auto host = ip4 | ipLiteral | regName;

    //  port        = *DIGIT
    const auto port = *digit;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // segment       = *pchar
    const auto segment = *pchar;

    // segment-nz    = 1*pchar
    const auto segmentNz = +pchar;

    // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    //               ; non-zero-length segment without any colon ":"
    const auto segmentNzNc = +(unreserved | pctEncoded | subDelims | '@');

    // path-abempty  = *( "/" segment )
    const auto pathAbempty = *('/' >> segment);

    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    const auto pathAbsolute = '/' >> -(segmentNz >> *('/' >> segment));

    // path-noscheme = segment-nz-nc *( "/" segment )
    const auto pathNoscheme = segmentNzNc >> *('/' >> segment);

    // path-rootless = segment-nz *( "/" segment )
    const auto pathRootless = segmentNz >> *('/' >> segment);

    // path-empty    = 0<pchar>
    const auto pathEmpty = x3::eps;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // path          = path-abempty    ; begins with "/" or is empty
    //               / path-absolute   ; begins with "/" but not "//"
    //               / path-noscheme   ; begins with a non-colon segment
    //               / path-rootless   ; begins with a segment
    //               / path-empty      ; zero characters
    const auto path = pathAbempty | pathAbsolute | pathNoscheme | pathRootless | pathEmpty;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <typename T> auto as = [](auto name, auto p) { return x3::rule<struct _, T> {name} = p; };

    template <class String>
    const auto r_userinfo = as<www::Userinfo<String>>("userinfo", x3::raw[username] >> -(':' >> x3::raw[userpasswd]));

    template <class String>
    const auto r_ip4 = as<networkNode::Ip4<String>>("ip4", x3::raw[ip4]);

    template <class String>
    const auto r_ip6 = as<networkNode::Ip6<String>>("ip6", x3::raw[ip6]);

    template <class String>
    const auto r_ipFuture = as<networkNode::IpFuture<String>>("ipFuture", x3::raw[ipFuture]);

    template <class String>
    const auto r_regName = as<networkNode::RegName<String>>("regName", x3::raw[regName]);

    template <class String>
    const auto r_host = as<networkNode::Host<String>>("host", r_ip4<String> | ('[' >> r_ip6<String> >> ']') | ('[' >> r_ipFuture<String> >> ']') | r_regName<String>);

    template <class String>
    const auto r_host4 = as<networkNode::Host<String>>("host", r_ip4<String> | r_regName<String>);

    template <class String>
    const auto r_host6 = as<networkNode::Host<String>>("host", ('[' >> r_ip6<String> >> ']') | ('[' >> r_ipFuture<String> >> ']') | r_regName<String>);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::Mailto<String>& val)
    {
        val._value = val._hierPart;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::Inproc<String>& val)
    {
        auto parseArtefact = std::tie(val._auth);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           x3::raw[*x3::char_] >>
                           x3::eoi
                       ),
                       parseArtefact);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::Local<String>& val)
    {
        auto parseArtefact = std::tie(val._auth);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           x3::raw[*x3::char_] >>
                           x3::eoi
                       ),
                       parseArtefact);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::File<String>& val)
    {
        auto parseArtefact = std::tie(val._auth, val._path);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           -x3::raw[host] >>
                           x3::raw[pathAbsolute] >>
                           x3::eoi
                       ),
                       parseArtefact);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <class> class Alt, class String>
    requires (std::is_same_v<uri::TCP<String>, Alt<String>> || std::is_same_v<uri::UDP<String>, Alt<String>>)
    bool phase2(Alt<String>& val)
    {
        auto parseArtefact = std::tie(val._auth._host, val._auth._port);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           r_host<String> >>
                           -(':' >> x3::raw[port]) >>
                           x3::eoi
                       ),
                       parseArtefact);

        if(!res && std::holds_alternative<uri::networkNode::Ip4<String>>(val._auth._host))
        {
            // вторая попытка отпарсить урл для случаев плохого префикса ip4, например 0.0.0.256. Такой конструкт должен быть расопзнан как regName
            res = x3::parse(
                           val._hierPart.begin(), val._hierPart.end(),
                           (
                               x3::lit("//") >>
                               r_regName<String> >>
                               -(':' >> x3::raw[port]) >>
                               x3::eoi
                           ),
                           parseArtefact);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <class> class Alt, class String>
    requires (std::is_same_v<uri::TCP4<String>, Alt<String>> || std::is_same_v<uri::UDP4<String>, Alt<String>>)
    bool phase2(Alt<String>& val)
    {
        auto parseArtefact = std::tie(val._auth._host, val._auth._port);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           r_host4<String> >>
                           -(':' >> x3::raw[port]) >>
                           x3::eoi
                       ),
                       parseArtefact);

        if(!res)
        {
            // вторая попытка отпарсить урл для случаев плохого префикса ip4, например 0.0.0.256. Такой конструкт должен быть расопзнан как regName
            res = x3::parse(
                           val._hierPart.begin(), val._hierPart.end(),
                           (
                               x3::lit("//") >>
                               r_regName<String> >>
                               -(':' >> x3::raw[port]) >>
                               x3::eoi
                           ),
                           parseArtefact);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <class> class Alt, class String>
    requires (std::is_same_v<uri::TCP6<String>, Alt<String>> || std::is_same_v<uri::UDP6<String>, Alt<String>>)
    bool phase2(Alt<String>& val)
    {
        auto parseArtefact = std::tie(val._auth._host, val._auth._port);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           r_host6<String> >>
                           -(':' >> x3::raw[port]) >>
                           x3::eoi
                       ),
                       parseArtefact);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::WWW<String>& val)
    {
        auto parseArtefact = std::tie(val._auth._userinfo, val._auth._networkNode._host, val._auth._networkNode._port, val._path);
        bool res = x3::parse(
                       val._hierPart.begin(), val._hierPart.end(),
                       (
                           x3::lit("//") >>
                           ((r_userinfo<String> >> '@') | x3::eps[([&]{ val._auth._userinfo = {}; })]) >> // https://github.com/boostorg/spirit/issues/378
                           r_host<String> >>
                           -(':' >> x3::raw[port]) >>
                           x3::raw[path] >>
                           x3::eoi
                       ),
                       parseArtefact);

        if(!res && std::holds_alternative<uri::networkNode::Ip4<String>>(val._auth._networkNode._host))
        {
            // вторая попытка отпарсить урл для случаев плохого префикса ip4, например 0.0.0.256. Такой конструкт должен быть расопзнан как regName
            res = x3::parse(
                           val._hierPart.begin(), val._hierPart.end(),
                           (
                               x3::lit("//") >>
                               ((r_userinfo<String> >> '@') | x3::eps[([&]{ val._auth._userinfo = {}; })]) >>
                               r_regName<String> >>
                               -(':' >> x3::raw[port]) >>
                               x3::raw[path] >>
                               x3::eoi
                           ),
                           parseArtefact);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::HTTP<String>& val)
    {
        return phase2(static_cast<uri::WWW<String>&>(val));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::HTTPS<String>& val)
    {
        return phase2(static_cast<uri::WWW<String>&>(val));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::FTP<String>& val)
    {
        return phase2(static_cast<uri::WWW<String>&>(val));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String>
    bool phase2(uri::FTPS<String>& val)
    {
        return phase2(static_cast<uri::WWW<String>&>(val));
    }
}

namespace
{
    template <class Src, class DstString>
    bool phase1(const Src& src, URI<DstString>& dst)
    {
        uri::Generic<DstString> generic;
        auto parseArtefact = std::tie(generic._scheme, generic._hierPart, generic._query, generic._fragment);
        bool res = x3::parse(
                       src.begin(), src.end(),
                       (
                           x3::raw[x3::alpha >> *(x3::alnum|'+'|'-'|'.')] >> ':' >>
                           x3::raw[*(x3::char_-'?'-'#')] >>
                           -('?' >> x3::raw[*(x3::char_-'#')]) >>
                           -('#' >> x3::raw[*x3::char_]) >>
                           x3::eoi
                       ),
                       parseArtefact);

        if(!res)
        {
            dst = uri::Unknown<DstString>{DstString{src}};
            return false;
        }

        switch(generic._scheme[0])
        {
        case 'i': //inproc
            if(6 == generic._scheme.size())
            {
                if("inproc"sv == generic._scheme)
                    return phase2(dst.template emplace<Inproc<DstString>>(std::move(generic)));
            }
            break;
        case 'l': //local
            if(5 == generic._scheme.size())
            {
                if("local"sv == generic._scheme)
                    return phase2(dst.template emplace<Local<DstString>>(std::move(generic)));
            }
            break;
        case 't': //tcp, tcp4, tcp6
            if(3 == generic._scheme.size())
            {
                if("tcp"sv == generic._scheme)
                    return phase2(dst.template emplace<TCP<DstString>>(std::move(generic)));
                break;
            }
            else if(4 == generic._scheme.size())
            {
                switch(generic._scheme[3])
                {
                case '4':
                    if("tcp4"sv == generic._scheme)
                        return phase2(dst.template emplace<TCP4<DstString>>(TCP<DstString>{std::move(generic)}));
                    break;
                case '6':
                    if("tcp6"sv == generic._scheme)
                        return phase2(dst.template emplace<TCP6<DstString>>(TCP<DstString>{std::move(generic)}));
                    break;
                }
            }
            break;
        case 'u': //udp, udp4, udp6
            if(3 == generic._scheme.size())
            {
                if("udp"sv == generic._scheme)
                    return phase2(dst.template emplace<UDP<DstString>>(std::move(generic)));
                break;
            }
            else if(4 == generic._scheme.size())
            {
                switch(generic._scheme[3])
                {
                case '4':
                    if("udp4"sv == generic._scheme)
                        return phase2(dst.template emplace<UDP4<DstString>>(UDP<DstString>{std::move(generic)}));
                    break;
                case '6':
                    if("udp6"sv == generic._scheme)
                        return phase2(dst.template emplace<UDP6<DstString>>(UDP<DstString>{std::move(generic)}));
                    break;
                }
            }
            break;
        case 'm': //mailto
            if("mailto"sv == generic._scheme)
                return phase2(dst.template emplace<Mailto<DstString>>(std::move(generic)));
            break;
        case 'f': //file, ftp, ftps
            if(3 == generic._scheme.size())
            {
                if("ftp"sv == generic._scheme)
                    return phase2(dst.template emplace<FTP<DstString>>(WWW<DstString>{std::move(generic)}));
            }
            else if(4 == generic._scheme.size())
            {
                switch(generic._scheme[1])
                {
                case 'i':
                    if("file"sv == generic._scheme)
                        return phase2(dst.template emplace<File<DstString>>(std::move(generic)));
                    break;
                case 't':
                    if("ftps"sv == generic._scheme)
                        return phase2(dst.template emplace<FTPS<DstString>>(WWW<DstString>{std::move(generic)}));
                    break;
                }
            }
            break;
        case 'h': //http, https
            if(4 == generic._scheme.size())
            {
                if("http"sv == generic._scheme)
                    return phase2(dst.template emplace<HTTP<DstString>>(WWW<DstString>{std::move(generic)}));
            }
            else if(5 == generic._scheme.size())
            {
                if("https"sv == generic._scheme)
                    return phase2(dst.template emplace<HTTPS<DstString>>(WWW<DstString>{std::move(generic)}));
            }
            break;
        }

        if(generic._hierPart.starts_with("//"sv))
            return phase2(dst.template emplace<WWW<DstString>>(std::move(generic)));

        dst = std::move(generic);
        return true;
    }
}

namespace dci::utils::uri
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, URI<std::string_view>& dst)
    {
        return phase1(src, dst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, URI<std::string>& dst)
    {
        return phase1(src, dst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, NetworkNode<std::string_view>& dst)
    {
        auto parseArtefact = std::tie(dst._host, dst._port);
        return x3::parse(
                    src.begin(), src.end(),
                    (
                        r_host<std::string_view> >>
                        ((':' >> x3::raw[port]) | x3::eps[([&]{dst._port={};})]) >>
                        x3::eoi
                    ),
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, NetworkNode<std::string>& dst)
    {
        auto parseArtefact = std::tie(dst._host, dst._port);
        return x3::parse(
                    src.begin(), src.end(),
                    (
                        r_host<std::string> >>
                        ((':' >> x3::raw[port]) | x3::eps[([&]{dst._port={};})]) >>
                        x3::eoi
                    ),
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::Host<std::string_view>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_host<std::string_view> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::Host<std::string>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_host<std::string> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::Ip4<std::string_view>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_ip4<std::string_view> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::Ip4<std::string>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_ip4<std::string> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::Ip6<std::string_view>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_ip6<std::string_view> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::Ip6<std::string>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_ip6<std::string> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::RegName<std::string_view>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_regName<std::string_view> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool parse(std::string_view src, networkNode::RegName<std::string>& dst)
    {
        auto parseArtefact = std::tie(dst);
        return x3::parse(
                    src.begin(), src.end(),
                    r_regName<std::string> >> x3::eoi,
                    parseArtefact);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool valid(std::string_view src)
    {
        URI<std::string_view> tmp;
        return parse(src, tmp) && !std::holds_alternative<uri::Unknown<std::string_view>>(tmp);
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class String>
        std::string_view schemeImpl(const URI<String>& src)
        {
            return std::visit([]<class Alt>(const Alt& alt) -> std::string_view {
                                  if constexpr(std::is_base_of_v<utils::uri::Generic<String>, Alt>)
                                      return alt._scheme;
                                  return {};
                              }, src);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class String>
        std::string_view hostImpl(const URI<String>& src)
        {
            return std::visit([]<class Alt>(const Alt& alt) -> std::string_view {
                                  if constexpr(std::is_same_v<File<String>, Alt>)
                                      return alt._auth ? *alt._auth : ""sv;
                                  if constexpr(std::is_same_v<TCP<String>, Alt> || std::is_base_of_v<TCP<String>, Alt>)
                                      return std::visit([](const auto& ipOrRegname) -> std::string_view  {
                                          return ipOrRegname;
                                      }, alt._auth._host);
                                  if constexpr(std::is_same_v<UDP<String>, Alt> || std::is_base_of_v<UDP<String>, Alt>)
                                      return std::visit([](const auto& ipOrRegname) -> std::string_view  {
                                          return ipOrRegname;
                                      }, alt._auth._host);
                                  if constexpr(std::is_same_v<WWW<String>, Alt> || std::is_base_of_v<WWW<String>, Alt>)
                                      return std::visit([](const auto& ipOrRegname) -> std::string_view  {
                                          return ipOrRegname;
                                      }, alt._auth._networkNode._host);
                                  return {};
                              }, src);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class String>
        std::string hostPortImpl(const URI<String>& src)
        {
            return std::visit([]<class Alt>(const Alt& alt) -> std::string {
                                  if constexpr(std::is_same_v<File<String>, Alt>)
                                      return alt._auth ? std::string{*alt._auth} : std::string{};
                                  if constexpr(std::is_same_v<TCP<String>, Alt> || std::is_base_of_v<TCP<String>, Alt>)
                                      return std::visit([&]<class IpOrRegname>(const IpOrRegname& ipOrRegname) -> std::string  {
                                          if constexpr(std::is_same_v<networkNode::Ip6<>, IpOrRegname> || std::is_same_v<networkNode::IpFuture<>, IpOrRegname>)
                                              return std::string{"["} + std::string{ipOrRegname} + "]";
                                          return std::string{ipOrRegname};
                                      }, alt._auth._host) + (alt._auth._port ? (":" + std::string{*alt._auth._port}) : std::string{});
                                  if constexpr(std::is_same_v<UDP<String>, Alt> || std::is_base_of_v<UDP<String>, Alt>)
                                  return std::visit([&]<class IpOrRegname>(const IpOrRegname& ipOrRegname) -> std::string  {
                                          if constexpr(std::is_same_v<networkNode::Ip6<>, IpOrRegname> || std::is_same_v<networkNode::IpFuture<>, IpOrRegname>)
                                              return std::string{"["} + std::string{ipOrRegname} + "]";
                                          return std::string{ipOrRegname};
                                      }, alt._auth._host) + (alt._auth._port ? (":" + std::string{*alt._auth._port}) : std::string{});
                                  if constexpr(std::is_same_v<WWW<String>, Alt> || std::is_base_of_v<WWW<String>, Alt>)
                                  return std::visit([&]<class IpOrRegname>(const IpOrRegname& ipOrRegname) -> std::string  {
                                          if constexpr(std::is_same_v<networkNode::Ip6<>, IpOrRegname> || std::is_same_v<networkNode::IpFuture<>, IpOrRegname>)
                                              return std::string{"["} + std::string{ipOrRegname} + "]";
                                          return std::string{ipOrRegname};
                                      }, alt._auth._networkNode._host) + (alt._auth._networkNode._port ? (":" + std::string{*alt._auth._networkNode._port}) : std::string{});
                                  return {};
                              }, src);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view scheme(std::string_view src)
    {
        URI<> tmp;
        if(!parse(src, tmp))
            return {};
        return schemeImpl(tmp);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view scheme(const URI<std::string_view>& src)
    {
        return schemeImpl(src);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view scheme(const URI<std::string>& src)
    {
        return schemeImpl(src);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view host(std::string_view src)
    {
        URI<> tmp;
        if(!parse(src, tmp))
            return {};
        return hostImpl(tmp);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view host(const URI<std::string_view>& src)
    {
        return hostImpl(src);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view host(const URI<std::string>& src)
    {
        return hostImpl(src);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string hostPort(std::string_view src)
    {
        URI<> tmp;
        if(!parse(src, tmp))
            return {};
        return hostPortImpl(tmp);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string hostPort(const URI<std::string_view>& src)
    {
        return hostPortImpl(src);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string hostPort(const URI<std::string>& src)
    {
        return hostPortImpl(src);
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class String>
        bool isCoverImpl(const URI<String>& base, const URI<String>& target)
        {
            // inproc < local < ip < regname

            if(std::holds_alternative<uri::Inproc<String>>(base))
                return true;

            if(std::holds_alternative<uri::Inproc<String>>(target))
                return false;

            if(std::holds_alternative<uri::Local<String>>(base))
                return true;

            if(std::holds_alternative<uri::Local<String>>(target))
                return false;

            return ip::isCover(hostImpl(base), hostImpl(target));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(std::string_view baseStr, std::string_view targetStr)
    {
        URI<std::string_view> base;
        if(!parse(baseStr, base) || std::holds_alternative<uri::Unknown<std::string_view>>(base))
            return false;

        URI<std::string_view> target;
        if(!parse(targetStr, target) || std::holds_alternative<uri::Unknown<std::string_view>>(target))
            return false;

        return isCoverImpl(base, target);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(const URI<std::string_view>& base, const URI<std::string_view>& target)
    {
        return isCoverImpl(base, target);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(const URI<std::string>& base, const URI<std::string>& target)
    {
        return isCoverImpl(base, target);
    }
}
