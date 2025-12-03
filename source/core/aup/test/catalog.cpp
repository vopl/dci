// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/aup.hpp>
using namespace dci::aup;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(aup, catalog_one)
{
    Catalog i;

    catalog::FilePtr f{new catalog::File};
    f->_dependencies.insert(Oid{});
    f->_kind = catalog::File::Kind::cmm;
    f->_path = "x/y/z";
    Oid oid = i.put(std::move(f));
    f = catalog::objectPtrCast<catalog::File>(i.get(oid));

    EXPECT_TRUE(!!f);
    EXPECT_EQ(f->_kind, catalog::File::Kind::cmm);
    EXPECT_EQ(f->_path, "x/y/z");
}
