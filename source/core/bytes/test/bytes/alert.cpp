// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "mkBig.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, alter)
{
    //advance
    {
        Bytes b;
        Alter a(b.begin());

        a.advance(-20);
        EXPECT_EQ(20u, b.size());

        a.advance(-20);
        EXPECT_EQ(40u, b.size());

        a.advance(10);
        EXPECT_EQ(40u, b.size());

        a.advance(30);
        EXPECT_EQ(40u, b.size());

        a.advance(10);
        EXPECT_EQ(50u, b.size());

        a.advance(10);
        EXPECT_EQ(60u, b.size());
    }

    //prepareWriteBuffer/commitWriteBuffer
    {
        Bytes b = mkBig();
        String copy = b.toString();
        const uint32 originalSize = b.size();

        Alter a(b.begin());

        while(a.pos() < originalSize * 2)
        {
            uint32 writeSize = 10;

            /////////
            copy.resize(std::max(copy.size(), size_t(a.pos()+writeSize)));
            std::fill_n(copy.begin()+a.pos(), writeSize, 'x');

            /////////
            while(writeSize)
            {
                uint32 bufSize;
                byte* buf = a.prepareWriteBuffer(bufSize);
                bufSize = std::min(bufSize, writeSize);
                memset(buf, 'x', bufSize);
                a.commitWriteBuffer(bufSize);
                writeSize -= bufSize;
            }

            EXPECT_EQ(copy, b.toString());
        }
    }

    //write
    {
        Bytes b = mkBig();
        String copy = b.toString();
        const uint32 originalSize = b.size();

        Alter a(b.begin());

        while(a.pos() < originalSize * 2)
        {
            uint32 writeSize = 10;

            /////////
            copy.resize(std::max(copy.size(), size_t(a.pos()+writeSize)));
            std::fill_n(copy.begin()+a.pos(), writeSize, 'x');

            /////////
            const char buf[] = "xxxxxxxxxx";
            a.write(buf, writeSize);

            EXPECT_EQ(copy, b.toString());
        }
    }

    //remove
    for(uint32 step=1; step<20; ++step)
    {
        Bytes b = mkBig();
        String copy = b.toString();

        Alter a(b.begin());
        while(!copy.empty())
        {
            uint32 removeSize = std::min(step, uint32(copy.size()));

            /////////
            a.remove(removeSize);

            /////////
            copy.erase(copy.begin(), copy.begin()+removeSize);

            EXPECT_EQ(copy, b.toString());
        }
    }

    //removeTo
    for(uint32 step=1; step<20; ++step)
    {
        Bytes b = mkBig();
        String copy = b.toString();

        Alter a(b.begin());
        while(!copy.empty())
        {
            uint32 removeSize = std::min(step, uint32(copy.size()));

            /////////
            String removed1(removeSize, 'B');
            a.removeTo(removed1.data(), removeSize);

            /////////
            String removed2 = copy.substr(0, removeSize);
            copy.erase(copy.begin(), copy.begin()+removeSize);

            EXPECT_EQ(removed1, removed2);
            EXPECT_EQ(copy, b.toString());
        }
    }

    //cow
    {
        Bytes original = mkBig();
        Bytes copy = original;

        copy.begin().advance(static_cast<int32>(copy.size() + 1));

        EXPECT_EQ(original, mkBig());
    }

    //cow
    {
        Bytes original = mkBig();
        Bytes copy = original;

        copy.begin().continuousData4Write();

        EXPECT_EQ(original, mkBig());
    }

    //cow
    {
        Bytes original = mkBig();
        Bytes copy = original;

        uint32 bufSize;
        copy.begin().prepareWriteBuffer(bufSize);
        copy.begin().commitWriteBuffer(bufSize);

        EXPECT_EQ(original, mkBig());
    }

    //cow
    {
        Bytes original = mkBig();
        Bytes copy = original;

        char c;
        copy.begin().write(&c, 1);

        EXPECT_EQ(original, mkBig());
    }

    //cow
    {
        Bytes original = mkBig();
        Bytes copy = original;

        copy.begin().remove(50);

        EXPECT_EQ(original, mkBig());
    }
}
