// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/utils/uri.hpp>

using namespace dci::utils;
using namespace std::string_view_literals;

namespace
{
    URI<> mk(std::string src)
    {
        URI<> res;
        bool pres = uri::parse(src, res);
        if(pres)
        {
            std::ostringstream out;
            out << res;
            EXPECT_EQ(src, out.str());
        }

        return res;
    }
}

namespace dci::utils::uri
{
    template <class Alt>
    bool operator==(const URI<>& l, const Alt& r)
    {
        if(!std::holds_alternative<Alt>(l))
            return false;
        return std::get<Alt>(l) == r;
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_genericBad)
{
    URI<> uri;
    ASSERT_FALSE(uri::parse(""sv, uri));
    ASSERT_FALSE(uri::parse(":"sv, uri));
    ASSERT_FALSE(uri::parse("$:"sv, uri));
    ASSERT_FALSE(uri::parse("+:"sv, uri));
    ASSERT_FALSE(uri::parse("-:"sv, uri));
    ASSERT_FALSE(uri::parse(".:"sv, uri));
    ASSERT_FALSE(uri::parse("0:"sv, uri));
    ASSERT_FALSE(uri::parse("5:"sv, uri));
    ASSERT_FALSE(uri::parse("9:"sv, uri));
    ASSERT_FALSE(uri::parse("aa$aa:"sv, uri));
    ASSERT_FALSE(uri::parse("?"sv, uri));
    ASSERT_FALSE(uri::parse("?query"sv, uri));
    ASSERT_FALSE(uri::parse("?:"sv, uri));
    ASSERT_FALSE(uri::parse("?query:"sv, uri));
    ASSERT_FALSE(uri::parse("#"sv, uri));
    ASSERT_FALSE(uri::parse("#fragment"sv, uri));
    ASSERT_FALSE(uri::parse("#:"sv, uri));
    ASSERT_FALSE(uri::parse("#fragment:"sv, uri));
    ASSERT_FALSE(uri::parse("/usr/bin/bash"sv, uri));
    ASSERT_FALSE(uri::parse("/usr/bin/bash:"sv, uri));
    ASSERT_FALSE(uri::parse("//localhost/usr/bin/bash:"sv, uri));
    ASSERT_FALSE(uri::parse(R"(\\localhost\usr\bin\bash)"sv, uri));
    ASSERT_FALSE(uri::parse(R"(\\localhost\usr\bin\bash:)"sv, uri));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_generic)
{
    ASSERT_EQ(mk("s+c+h-e-m..e:"), (uri::Generic<>{"s+c+h-e-m..e", ""}));
    ASSERT_EQ(mk("a0+-.:"), (uri::Generic<>{"a0+-.", ""}));
    ASSERT_EQ(mk("a+++:"), (uri::Generic<>{"a+++", ""}));
    ASSERT_EQ(mk("a---:"), (uri::Generic<>{"a---", ""}));
    ASSERT_EQ(mk("a...:"), (uri::Generic<>{"a...", ""}));
    ASSERT_EQ(mk("a999:"), (uri::Generic<>{"a999", ""}));

    ASSERT_EQ(mk("scheme:"), (uri::Generic<>{"scheme", ""}));
    ASSERT_EQ(mk("scheme::"), (uri::Generic<>{"scheme", ":"}));
    ASSERT_EQ(mk("scheme:?"), (uri::Generic<>{"scheme", "", {""}}));
    ASSERT_EQ(mk("scheme:#"), (uri::Generic<>{"scheme", "", {}, {""}}));
    ASSERT_EQ(mk("scheme:?#"), (uri::Generic<>{"scheme", "", {""}, {""}}));

    ASSERT_EQ(mk("scheme:hier-part"), (uri::Generic<>{"scheme", "hier-part"}));
    ASSERT_EQ(mk("scheme:hier-part?query"), (uri::Generic<>{"scheme", "hier-part", {"query"}}));
    ASSERT_EQ(mk("scheme:hier-part?query?q2"), (uri::Generic<>{"scheme", "hier-part", {"query?q2"}}));
    ASSERT_EQ(mk("scheme:hier-part#fragment"), (uri::Generic<>{"scheme", "hier-part", {}, {"fragment"}}));
    ASSERT_EQ(mk("scheme:hier-part#fragment#f2"), (uri::Generic<>{"scheme", "hier-part", {}, {"fragment#f2"}}));
    ASSERT_EQ(mk("scheme:hier-part?query#fragment"), (uri::Generic<>{"scheme", "hier-part", {"query"}, {"fragment"}}));
    ASSERT_EQ(mk("scheme:hier-part?query?q2#fragment"), (uri::Generic<>{"scheme", "hier-part", {"query?q2"}, {"fragment"}}));
    ASSERT_EQ(mk("scheme:hier-part?query?q2#fragment#f2"), (uri::Generic<>{"scheme", "hier-part", {"query?q2"}, {"fragment#f2"}}));

    ASSERT_TRUE(std::holds_alternative<uri::Unknown<>>(mk("scheme")));
    ASSERT_TRUE(std::holds_alternative<uri::Unknown<>>(mk("?query")));
    ASSERT_TRUE(std::holds_alternative<uri::Unknown<>>(mk(":?query")));
    ASSERT_TRUE(std::holds_alternative<uri::Unknown<>>(mk("xx(yy):?query")));
    ASSERT_TRUE(std::holds_alternative<uri::Unknown<>>(mk("//blabla")));
    ASSERT_TRUE(std::holds_alternative<uri::Unknown<>>(mk("\\blabla")));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_mailto)
{
    ASSERT_EQ(mk("mailto:"), (uri::Mailto<>{{"mailto", "", {}, {}}, ""}));
    ASSERT_EQ(mk("mailto:jhohn"), (uri::Mailto<>{{"mailto", "jhohn", {}, {}}, "jhohn"}));
    ASSERT_EQ(mk("mailto:jhohn?q"), (uri::Mailto<>{{"mailto", "jhohn", {"q"}, {}}, "jhohn"}));
    ASSERT_EQ(mk("mailto:jhohn#f"), (uri::Mailto<>{{"mailto", "jhohn", {}, {"f"}}, "jhohn"}));
    ASSERT_EQ(mk("mailto:jhohn?q#f"), (uri::Mailto<>{{"mailto", "jhohn", {"q"}, {"f"}}, "jhohn"}));
    ASSERT_EQ(mk("mailto:jhohn@smith.xyz"), (uri::Mailto<>{{"mailto", "jhohn@smith.xyz", {}, {}}, "jhohn@smith.xyz"}));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_fileBad)
{
    URI<> uri;
    ASSERT_FALSE(uri::parse("file:"sv, uri));
    ASSERT_FALSE(uri::parse("file::"sv, uri));
    ASSERT_FALSE(uri::parse("file:www.example.com/path"sv, uri));
    ASSERT_FALSE(uri::parse("file:."sv, uri));
    ASSERT_FALSE(uri::parse("file:+++"sv, uri));
    ASSERT_FALSE(uri::parse("file:_"sv, uri));
    ASSERT_FALSE(uri::parse("file:/host/path"sv, uri));
    ASSERT_FALSE(uri::parse("file:host/path"sv, uri));
    ASSERT_FALSE(uri::parse(R"(file:\host\path)"sv, uri));
    ASSERT_FALSE(uri::parse(R"(file:\\host\\path)"sv, uri));
    ASSERT_FALSE(uri::parse("file://localhost/c|/WINDOWS/clock.avi"sv, uri));
    ASSERT_FALSE(uri::parse("file:///c|/WINDOWS/clock.avi"sv, uri));
    ASSERT_FALSE(uri::parse("file:///C:/exampleㄓ.txt"sv, uri));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_file)
{
    ASSERT_EQ(mk("file://host/fi/le"), (uri::File<>{{"file", "//host/fi/le", {}, {}}, {"host"}, "/fi/le"}));
    ASSERT_EQ(mk("file:///fi/le"), (uri::File<>{{"file", "///fi/le", {}, {}}, {}, "/fi/le"}));

    ASSERT_EQ(mk("file://localhost/etc/fstab"), (uri::File<>{{"file", "//localhost/etc/fstab", {}, {}}, {"localhost"}, "/etc/fstab"}));
    ASSERT_EQ(mk("file:///etc/fstab"), (uri::File<>{{"file", "///etc/fstab", {}, {}}, {}, "/etc/fstab"}));
    ASSERT_EQ(mk("file:///etc/./fstab"), (uri::File<>{{"file", "///etc/./fstab", {}, {}}, {}, "/etc/./fstab"}));
    ASSERT_EQ(mk("file:///etc/../etc/fstab"), (uri::File<>{{"file", "///etc/../etc/fstab", {}, {}}, {}, "/etc/../etc/fstab"}));
    ASSERT_EQ(mk("file://nnsc.nsf.net/rfc/rfc959.txt"), (uri::File<>{{"file", "//nnsc.nsf.net/rfc/rfc959.txt", {}, {}}, {"nnsc.nsf.net"}, "/rfc/rfc959.txt"}));
    ASSERT_EQ(mk("file://localhost/var/log/system.log"), (uri::File<>{{"file", "//localhost/var/log/system.log", {}, {}}, {"localhost"}, "/var/log/system.log"}));
    ASSERT_EQ(mk("file:///var/log/system.log"), (uri::File<>{{"file", "///var/log/system.log", {}, {}}, {}, "/var/log/system.log"}));
    ASSERT_EQ(mk("file://applib/products/a-b/abc_9/4148.920a/media/start.swf"), (uri::File<>{{"file", "//applib/products/a-b/abc_9/4148.920a/media/start.swf", {}, {}}, {"applib"}, "/products/a-b/abc_9/4148.920a/media/start.swf"}));
    ASSERT_EQ(mk("file:///C:/Documents%20and%20Settings/davris/FileSchemeURIs.doc"), (uri::File<>{{"file", "///C:/Documents%20and%20Settings/davris/FileSchemeURIs.doc", {}, {}}, {}, "/C:/Documents%20and%20Settings/davris/FileSchemeURIs.doc"}));
    ASSERT_EQ(mk("file:///c:/windows/My%20Documents%20100%2520/foo.txt"), (uri::File<>{{"file", "///c:/windows/My%20Documents%20100%2520/foo.txt", {}, {}}, {}, "/c:/windows/My%20Documents%20100%2520/foo.txt"}));
    ASSERT_EQ(mk("file://server/share/My%20Documents%20100%2520/foo.txt"), (uri::File<>{{"file", "//server/share/My%20Documents%20100%2520/foo.txt", {}, {}}, {"server"}, "/share/My%20Documents%20100%2520/foo.txt"}));

    ASSERT_EQ(mk("file:///etc/./fstab?q"), (uri::File<>{{"file", "///etc/./fstab", {"q"}, {}}, {}, "/etc/./fstab"}));
    ASSERT_EQ(mk("file:///etc/./fstab#f"), (uri::File<>{{"file", "///etc/./fstab", {}, {"f"}}, {}, "/etc/./fstab"}));
    ASSERT_EQ(mk("file:///etc/./fstab?q#f"), (uri::File<>{{"file", "///etc/./fstab", {"q"}, {"f"}}, {}, "/etc/./fstab"}));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_tcpBad)
{
    URI<> uri;
    ASSERT_FALSE(uri::parse("tcp:"sv, uri));
    ASSERT_FALSE(uri::parse("tcp:0.0.0.0"sv, uri));
    ASSERT_FALSE(uri::parse("tcp:example.com"sv, uri));
    ASSERT_FALSE(uri::parse("tcp:[::]:8080"sv, uri));

    ASSERT_FALSE(uri::parse("tcp://as%sa"sv, uri));
    ASSERT_FALSE(uri::parse("tcp://as%%sa"sv, uri));
    ASSERT_FALSE(uri::parse("tcp://as^sa"sv, uri));
    ASSERT_FALSE(uri::parse("tcp://as`sa"sv, uri));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_tcp)
{
    ASSERT_EQ(mk("tcp://example.com"), (uri::TCP<>{{"tcp", "//example.com", {}, {}}, {uri::networkNode::RegName{{"example.com"}}, {}}}));
    ASSERT_EQ(mk("tcp://-._~"), (uri::TCP<>{{"tcp", "//-._~", {}, {}}, {uri::networkNode::RegName{{"-._~"}}, {}}}));
    ASSERT_EQ(mk("tcp://%ff%ee"), (uri::TCP<>{{"tcp", "//%ff%ee", {}, {}}, {uri::networkNode::RegName{{"%ff%ee"}}, {}}}));
    ASSERT_EQ(mk("tcp://!$&'()*+,;="), (uri::TCP<>{{"tcp", "//!$&'()*+,;=", {}, {}}, {uri::networkNode::RegName{{"!$&'()*+,;="}}, {}}}));

    ASSERT_EQ(mk("tcp://0.0.0.0"), (uri::TCP<>{{"tcp", "//0.0.0.0", {}, {}}, {uri::networkNode::Ip4{{"0.0.0.0"}}, {}}}));
    ASSERT_EQ(mk("tcp://0.127.255.0"), (uri::TCP<>{{"tcp", "//0.127.255.0", {}, {}}, {uri::networkNode::Ip4{{"0.127.255.0"}}, {}}}));
    ASSERT_EQ(mk("tcp://0.127.255.256"), (uri::TCP<>{{"tcp", "//0.127.255.256", {}, {}}, {uri::networkNode::RegName{{"0.127.255.256"}}, {}}}));

    ASSERT_EQ(mk("tcp://[::]"), (uri::TCP<>{{"tcp", "//[::]", {}, {}}, {uri::networkNode::Ip6{{"::"}}, {}}}));
    ASSERT_EQ(mk("tcp://[2001:db8::7]"), (uri::TCP<>{{"tcp", "//[2001:db8::7]", {}, {}}, {uri::networkNode::Ip6{{"2001:db8::7"}}, {}}}));
    ASSERT_EQ(mk("tcp://[1:1::1]"), (uri::TCP<>{{"tcp", "//[1:1::1]", {}, {}}, {uri::networkNode::Ip6{{"1:1::1"}}, {}}}));
    ASSERT_EQ(mk("tcp://[1:1::]"), (uri::TCP<>{{"tcp", "//[1:1::]", {}, {}}, {uri::networkNode::Ip6{{"1:1::"}}, {}}}));
    ASSERT_EQ(mk("tcp://[1::1]"), (uri::TCP<>{{"tcp", "//[1::1]", {}, {}}, {uri::networkNode::Ip6{{"1::1"}}, {}}}));

    ASSERT_EQ(mk("tcp://example.com:"), (uri::TCP<>{{"tcp", "//example.com:", {}, {}}, {uri::networkNode::RegName{{"example.com"}}, {""}}}));
    ASSERT_EQ(mk("tcp://0.0.0.0:"), (uri::TCP<>{{"tcp", "//0.0.0.0:", {}, {}}, {uri::networkNode::Ip4{{"0.0.0.0"}}, {""}}}));
    ASSERT_EQ(mk("tcp://[::]:"), (uri::TCP<>{{"tcp", "//[::]:", {}, {}}, {uri::networkNode::Ip6{{"::"}}, {""}}}));

    ASSERT_EQ(mk("tcp://example.com:80"), (uri::TCP<>{{"tcp", "//example.com:80", {}, {}}, {uri::networkNode::RegName{{"example.com"}}, {"80"}}}));
    ASSERT_EQ(mk("tcp://0.0.0.0:80"), (uri::TCP<>{{"tcp", "//0.0.0.0:80", {}, {}}, {uri::networkNode::Ip4{{"0.0.0.0"}}, {"80"}}}));
    ASSERT_EQ(mk("tcp://[::]:80"), (uri::TCP<>{{"tcp", "//[::]:80", {}, {}}, {uri::networkNode::Ip6{{"::"}}, {"80"}}}));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_tcp4Bad)
{
    URI<> uri;
    ASSERT_FALSE(uri::parse("tcp4://[::]"sv, uri));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_tcp4)
{
    ASSERT_EQ(mk("tcp4://example.com"), (uri::TCP4<>{{{"tcp4", "//example.com", {}, {}}, {uri::networkNode::RegName{{"example.com"}}, {}}}}));
    ASSERT_EQ(mk("tcp4://0.0.0.0"), (uri::TCP4<>{{{"tcp4", "//0.0.0.0", {}, {}}, {uri::networkNode::Ip4{{"0.0.0.0"}}, {}}}}));
    ASSERT_EQ(mk("tcp4://0.0.0.0:80"), (uri::TCP4<>{{{"tcp4", "//0.0.0.0:80", {}, {}}, {uri::networkNode::Ip4{{"0.0.0.0"}}, {"80"}}}}));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_tcp6Bad)
{
    URI<> uri;
    //ASSERT_FALSE(uri::parse("tcp6://0.0.0.0"sv, uri));// это будет regName
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_tcp6)
{
    ASSERT_EQ(mk("tcp6://example.com"), (uri::TCP6<>{{{"tcp6", "//example.com", {}, {}}, {uri::networkNode::RegName{{"example.com"}}, {}}}}));
    ASSERT_EQ(mk("tcp6://[::]"), (uri::TCP6<>{{{"tcp6", "//[::]", {}, {}}, {uri::networkNode::Ip6{{"::"}}, {}}}}));
    ASSERT_EQ(mk("tcp6://[::]:80"), (uri::TCP6<>{{{"tcp6", "//[::]:80", {}, {}}, {uri::networkNode::Ip6{{"::"}}, {"80"}}}}));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, uri_www)
{
    ASSERT_EQ(mk("xyz://host"), (uri::WWW<>{{"xyz", "//host", {}, {}}, {{}, {uri::networkNode::RegName{{"host"}}, {}}}, ""}));
    ASSERT_EQ(mk("xyz://username@host"), (uri::WWW<>{{"xyz", "//username@host", {}, {}}, {{{"username", {}}}, {uri::networkNode::RegName{{"host"}}, {}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host"), (uri::WWW<>{{"xyz", "//username:password@host", {}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {}}}, ""}));
    ASSERT_EQ(mk("xyz://:password@host"), (uri::WWW<>{{"xyz", "//:password@host", {}, {}}, {{{"", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {}}}, ""}));
    ASSERT_EQ(mk("xyz://:@host"), (uri::WWW<>{{"xyz", "//:@host", {}, {}}, {{{"", {""}}}, {uri::networkNode::RegName{{"host"}}, {}}}, ""}));
    ASSERT_EQ(mk("xyz://@host"), (uri::WWW<>{{"xyz", "//@host", {}, {}}, {{{"", {}}}, {uri::networkNode::RegName{{"host"}}, {}}}, ""}));

    ASSERT_EQ(mk("xyz://username:password@host:"), (uri::WWW<>{{"xyz", "//username:password@host:", {}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {""}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host:123"), (uri::WWW<>{{"xyz", "//username:password@host:123", {}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host:123/"), (uri::WWW<>{{"xyz", "//username:password@host:123/", {}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, "/"}));
    ASSERT_EQ(mk("xyz://username:password@host:123/a/b/c"), (uri::WWW<>{{"xyz", "//username:password@host:123/a/b/c", {}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, "/a/b/c"}));

    ASSERT_EQ(mk("xyz://username:password@host:?x=y"), (uri::WWW<>{{"xyz", "//username:password@host:", {"x=y"}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {""}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host:123?x=y"), (uri::WWW<>{{"xyz", "//username:password@host:123", {"x=y"}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host:123/?x=y"), (uri::WWW<>{{"xyz", "//username:password@host:123/", {"x=y"}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, "/"}));
    ASSERT_EQ(mk("xyz://username:password@host:123/a/b/c?x=y"), (uri::WWW<>{{"xyz", "//username:password@host:123/a/b/c", {"x=y"}, {}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, "/a/b/c"}));

    ASSERT_EQ(mk("xyz://username:password@host:?x=y#xyz"), (uri::WWW<>{{"xyz", "//username:password@host:", {"x=y"}, {"xyz"}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {""}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host:123?x=y#xyz"), (uri::WWW<>{{"xyz", "//username:password@host:123", {"x=y"}, {"xyz"}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, ""}));
    ASSERT_EQ(mk("xyz://username:password@host:123/?x=y#xyz"), (uri::WWW<>{{"xyz", "//username:password@host:123/", {"x=y"}, {"xyz"}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, "/"}));
    ASSERT_EQ(mk("xyz://username:password@host:123/a/b/c?x=y#xyz"), (uri::WWW<>{{"xyz", "//username:password@host:123/a/b/c", {"x=y"}, {"xyz"}}, {{{"username", {"password"}}}, {uri::networkNode::RegName{{"host"}}, {"123"}}}, "/a/b/c"}));
}
