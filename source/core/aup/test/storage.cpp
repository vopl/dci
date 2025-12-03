// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/aup.hpp>
using namespace dci::aup;

#include <dci/utils/b2h.hpp>
#include <dci/crypto.hpp>
#include <filesystem>
using namespace dci;

namespace
{
    Oid rndOid()
    {
        Oid res;
        crypto::rnd::generate(res.data(), res.size());
        return res;
    }

    Bytes makeBlob(const Oid& oid)
    {
        Bytes res;
        bytes::Alter a{res.end()};

        uint8 size = oid[0];

        for(uint8 i{0}; i<size; ++i)
        {
            a.write(&oid[i % oid.size()], 1);
        }

        return res;
    }

    bool checkBlob(const Oid& oid, const Bytes& blob)
    {
        bytes::Cursor c{blob.begin()};
        uint8 size = oid[0];

        if(size != blob.size())
        {
            return false;
        }

        for(uint8 i{0}; i<size; ++i)
        {
            uint8 one {};
            c.read(&one, 1);
            if(oid[i % oid.size()] != one)
            {
                return false;
            }
        }

        return true;
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(aup, storage)
{
    Storage s;

    s.reset((std::filesystem::temp_directory_path() / utils::b2h(crypto::rnd::generate(32))).string());

    std::set<Oid> oids;
    for(std::size_t i{}; i<10; ++i)
    {
        Oid oid = rndOid();
        s.put(oid, makeBlob(oid));
        oids.insert(oid);
    }

    for(const Oid& oid : s.enumerate())
    {
        //std::cout<<utils::b2h(e.get())<<std::endl;
        EXPECT_TRUE(checkBlob(oid, *s.get(oid)));
        s.del(oid);
        oids.erase(oid);
    }
    EXPECT_TRUE(oids.empty());

    s.delAll();
}
