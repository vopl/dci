// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <cstddef>
#include <dci/test.hpp>
#include <dci/bytes.hpp>

using namespace dci;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, ctor)
{
    EXPECT_EQ(Bytes{}.toString(), String{});
    EXPECT_EQ(Bytes{"42"}.toString(), String{"42"});
    EXPECT_EQ((Bytes{"42", 2}.toString()), String{"42"});
    EXPECT_EQ((Bytes{"\0\0\0", 3}.toString()), (String(std::size_t{3}, '\0')));

    EXPECT_EQ(Bytes{Bytes{"42"}}.toString(), String{"42"});

    Bytes b1{"42"};
    EXPECT_EQ(Bytes{b1}.toString(), String{"42"});
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, eq)
{
    Bytes b1{"42"};
    Bytes b2;

    b2 = b1;
    EXPECT_EQ(b2.toString(), String{"42"});
    EXPECT_EQ(b1.toString(), String{"42"});

    b2 = std::move(b1);
    EXPECT_EQ(b2.toString(), String{"42"});
    EXPECT_EQ(b1.toString(), String{});
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, cmp)
{
    Bytes b1{"42"};
    Bytes b2{"43"};
    Bytes b3{"43"};
    Bytes b4{"435"};

    EXPECT_TRUE(b2 == b3);
    EXPECT_TRUE(b1 != b2);
    EXPECT_TRUE(b1 <  b2);
    EXPECT_TRUE(b2 >  b1);
    EXPECT_TRUE(b1 <= b1);
    EXPECT_TRUE(b1 <= b2);
    EXPECT_TRUE(b2 >= b1);
    EXPECT_TRUE(b2 >= b2);

    EXPECT_TRUE(b3 != b4);
    EXPECT_TRUE(b3 < b4);
    EXPECT_TRUE(b3 <= b4);
    EXPECT_TRUE(b4 > b3);
    EXPECT_TRUE(b4 >= b3);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, clone)
{
    EXPECT_EQ(Bytes{static_cast<const Bytes&>(Bytes{"42"})}.toString(), String{"42"});
    EXPECT_EQ(Bytes{Bytes{"42"}}.toString(), String{"42"});
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, empty)
{
    EXPECT_TRUE(Bytes{}.empty());
    EXPECT_TRUE(Bytes{""}.empty());
    EXPECT_TRUE((Bytes{"", 0}.empty()));

    EXPECT_FALSE(Bytes{"42"}.empty());
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, size)
{
    EXPECT_EQ(Bytes{}.size(), 0u);
    EXPECT_EQ(Bytes{""}.size(), 0u);
    EXPECT_EQ(Bytes{"42"}.size(), 2u);
    EXPECT_EQ(Bytes{Bytes{"42"}}.size(), 2u);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, clear)
{
    Bytes b1{"42"};
    b1.clear();

    EXPECT_EQ(b1.size(), 0u);
    EXPECT_TRUE(b1.empty());
}
