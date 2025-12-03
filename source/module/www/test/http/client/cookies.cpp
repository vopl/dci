// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/poll.hpp>
#include <dci/cmt.hpp>
#include <dci/exception.hpp>
#include "www.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;
using namespace dci::primitives;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client_cookies, fromResponse)
{
    auto process = [](String domain, String path, String setCookieStr) -> www::http::client::cookies::Entry
    {
        www::http::client::Cookies<> api = *testManager()->createService<www::http::client::Cookies<>>();
        api->fromResponse(domain, path, List<String>{setCookieStr});

        List<www::http::client::cookies::Entry> entries = api->get(www::http::client::cookies::entry::Id{}).value();
        EXPECT_EQ(entries.size(), 1);
        www::http::client::cookies::Entry entry = entries[0];

        api->del(www::http::client::cookies::entry::Id{});
        entries = api->get(www::http::client::cookies::entry::Id{}).value();
        EXPECT_EQ(entries.size(), 0);
        return entry;
    };

    www::http::client::cookies::Entry entry;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("127.0.0.1", "", "name=");
    ASSERT_EQ(entry.id.name, "name");
    ASSERT_EQ(entry.value, "");

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "name=value");
    ASSERT_EQ(entry.id.name, "name");
    ASSERT_EQ(entry.value, "value");

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "!#$%^&*_+`-name!#$%^&*_+`-=");
    ASSERT_EQ(entry.id.name, "!#$%^&*_+`-name!#$%^&*_+`-");

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=~`!@#$%^&*()_-+=[]{}|':/?.><mnbvcxzasdfghjklpoiuytrewq");
    ASSERT_EQ(entry.id.name, "n");
    ASSERT_EQ(entry.value, "~`!@#$%^&*()_-+=[]{}|':/?.><mnbvcxzasdfghjklpoiuytrewq");

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=");
    ASSERT_EQ(entry.id.domain, ".");
    ASSERT_EQ(entry.id.path, "/");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process(".", "/", "n=");
    ASSERT_EQ(entry.id.domain, ".");
    ASSERT_EQ(entry.id.path, "/");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process(".", "////file", "n=");
    ASSERT_EQ(entry.id.domain, ".");
    ASSERT_EQ(entry.id.path, "/");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "/dir/dir/", "n=");
    ASSERT_EQ(entry.id.domain, ".");
    ASSERT_EQ(entry.id.path, "/dir/dir/");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("a", "/dir/dir/file", "n=");
    ASSERT_EQ(entry.id.domain, ".a.");
    ASSERT_EQ(entry.id.path, "/dir/dir/");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("a.BbBb.\xd1\x80\xd1\x84", "/a/\xd1\x80\xd1\x84/ru\xd1\x80\xd1\x84.txt", "n=");
    ASSERT_EQ(entry.id.domain, ".a.bbbb.xn--p1ai.");
    ASSERT_EQ(entry.id.path, "/a/\xd1\x80\xd1\x84/");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 now1 = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::utc_clock::now().time_since_epoch()).count();
    entry = process("", "", "n=");
    uint64 now2 = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::utc_clock::now().time_since_epoch()).count();

    ASSERT_GE(entry.creationTime, now1);
    ASSERT_LE(entry.creationTime, now2);
    ASSERT_EQ(entry.lastAccessTime, entry.creationTime);
    ASSERT_GT(entry.expiryTime, entry.creationTime+60*60*24*365*10);
    ASSERT_EQ(entry.persistent, false);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; expires=Sun, 28 Jul 2024 16:56:32 GMT");
    ASSERT_EQ(entry.expiryTime, 1722185792);
    ASSERT_EQ(entry.persistent, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; max-age=123456");
    ASSERT_EQ(entry.expiryTime, entry.creationTime + 123456);
    ASSERT_EQ(entry.persistent, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; domain=");
    ASSERT_EQ(entry.id.hostOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; domain=.");
    ASSERT_EQ(entry.id.hostOnly, false);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("z.a.b.c", "", "n=; domain=.a.b.c");
    ASSERT_EQ(entry.id.hostOnly, false);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=");
    ASSERT_EQ(entry.secure, false);
    ASSERT_EQ(entry.id.hostOnly, true);
    ASSERT_EQ(entry.httpOnly, false);
    ASSERT_EQ(entry.partitioned, false);
    ASSERT_EQ(entry.sameSite, www::http::client::cookies::SameSite::null);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; secure");
    ASSERT_EQ(entry.secure, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; HttpONLY");
    ASSERT_EQ(entry.httpOnly, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; PartiTiOnEd");
    ASSERT_EQ(entry.partitioned, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; SameSite=Lax");
    ASSERT_EQ(entry.sameSite, www::http::client::cookies::SameSite::lax);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; Samesite=Strict");
    ASSERT_EQ(entry.sameSite, www::http::client::cookies::SameSite::strict);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entry = process("", "", "n=; SAMESITE=None");
    ASSERT_EQ(entry.sameSite, www::http::client::cookies::SameSite::none);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client_cookies, get_byDomain)
{
    www::http::client::Cookies<> api = *testManager()->createService<www::http::client::Cookies<>>();
    api->fromResponse("example.com", "", List<String>
                      {
                          "n1=",

                          "n2=; Domain=",
                          "n3=; Domain=com",
                          "n4=; Domain=example.com",
                          "n5=; Domain=www.example.com",
                          "n6=; Domain=bar.example.com",

                          "n7=; Domain=org",
                          "n8=; Domain=other.org",
                          "n9=; Domain=www.other.org",

                          "n10=; Domain=other.com",
                          "n11=; Domain=www.other.com",
                      });

    List<www::http::client::cookies::Entry> entries;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{}).value();
    ASSERT_EQ(entries.size(), 4);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "com", ""}).value();
    ASSERT_EQ(entries.size(), 4);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "example.com", ""}).value();
    ASSERT_EQ(entries.size(), 3);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "www.example.com", ""}).value();
    ASSERT_EQ(entries.size(), 0);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "other.com", ""}).value();
    ASSERT_EQ(entries.size(), 0);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client_cookies, get_byPath)
{
    www::http::client::Cookies<> api = *testManager()->createService<www::http::client::Cookies<>>();
    api->fromResponse("", "/a/b", List<String>
                      {
                          "n1=",

                          "n2=; path=",
                          "n3=; path=/x",
                          "n4=; path=/x/",
                          "n5=; path=/x/y",
                          "n6=; path=/x/y/",

                          "n7=; path=/a",
                          "n8=; path=/a/",
                          "n9=; path=/a/b",
                          "n10=; path=/a/b/",
                          "n11=; path=/a/b/c",
                          "n12=; path=/a/b/c/",
                      });

    List<www::http::client::cookies::Entry> entries;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{}).value();
    ASSERT_EQ(entries.size(), 12);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/"}).value();
    ASSERT_EQ(entries.size(), 12);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/x"}).value();
    ASSERT_EQ(entries.size(), 4);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/x/"}).value();
    ASSERT_EQ(entries.size(), 4);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/x/y"}).value();
    ASSERT_EQ(entries.size(), 2);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/a"}).value();
    ASSERT_EQ(entries.size(), 8);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/a/b"}).value();
    ASSERT_EQ(entries.size(), 4);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    entries = api->get(www::http::client::cookies::entry::Id{"", false, "", "/a/b/c"}).value();
    ASSERT_EQ(entries.size(), 2);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client_cookies, toRequest)
{
    www::http::client::Cookies<> api = *testManager()->createService<www::http::client::Cookies<>>();
    api->fromResponse("", "/a/b", List<String>
                      {
                          "n1=",

                          "n2=v2; path=",
                          "n3=; path=/x",
                          "n4=; path=/x/",
                          "n5=; path=/x/y",
                          "n6=; path=/x/y/",

                          "n7=; path=/a",
                          "n8=; path=/a/",
                          "n9=; path=/a/b",
                          "n10=; path=/a/b/",
                          "n11=; path=/a/b/c",
                          "n12=; path=/a/b/c/",
                      });

    auto join = [](auto list, auto delim)
    {
        String res;
        bool first = true;
        for(const auto& element: list)
        {
            if(element.empty())
                continue;

            if(first)
                first = false;
            else
                res += delim;

            res += element;
        }
        return res;
    };

    ASSERT_EQ(join(api->toRequest("", "/a/b/c/", true, true).value(), "; "), "n11=; n12=; n9=; n10=; n1=; n2=v2; n7=; n8=");
    ASSERT_EQ(join(api->toRequest("", "/a/b/c" , true, true).value(), "; "),             "n9=; n10=; n1=; n2=v2; n7=; n8=");
    ASSERT_EQ(join(api->toRequest("", "/a/"    , true, true).value(), "; "),                        "n1=; n2=v2; n7=; n8=");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client_cookies, serialization)
{
    www::http::client::Cookies<> api = *testManager()->createService<www::http::client::Cookies<>>();
    api->fromResponse("", "/a/b", List<String>
                      {
                          "n1=; Max-Age=60",
                          "n2=v2; path=/e/r/t",
                          "n3=blabla; Secure",
                      });

    Bytes blob = api->serialize().value();
    // LOGD(blob.toHex());
    // LOGD(blob.toString());

    www::http::client::Cookies<> api2 = *testManager()->createService<www::http::client::Cookies<>>();
    api2->deserialize(std::move(blob)).value();

    ASSERT_EQ(api->get(www::http::client::cookies::entry::Id{}).value(), api2->get(www::http::client::cookies::entry::Id{}).value());
}
