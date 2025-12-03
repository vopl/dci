// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/idl/interface.hpp>
#include <dci/qml/qmeta.hpp>
#include "idl-victim.hpp"

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

namespace
{
    template <class sd, auto name>
    void checkOne()
    {
        uint idx = sd::template _indexFor<name>;

        const uint* qt = sd::qt();

        uint ofs = qt[idx*2+0];
        uint len = qt[idx*2+1];

#if defined(__GNUC__) && !defined(__clang__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstringop-overread"
#endif

        const char* csz = reinterpret_cast<const char*>(qt) + ofs;
        EXPECT_EQ(csz, sd::getStr(idx));
        EXPECT_EQ(len, sd::getLen(idx));
        EXPECT_EQ(strlen(csz), len);

#if defined(__GNUC__) && !defined(__clang__)
#   pragma GCC diagnostic pop
#endif

        EXPECT_EQ(name.size()-1, len);
        EXPECT_EQ(0, strncmp(csz, name.data(), len));
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(qml, qmeta_stringdata_struct)
{
    using sd = dci::qml::qmeta::Stringdata<a::b::c::S1>;

    checkOne<sd, introspection::typeName<a::b::c::S1>>();
    checkOne<sd, introspection::fieldName<a::b::c::S1, 0>>();
    checkOne<sd, introspection::fieldName<a::b::c::S1, 1>>();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(qml, qmeta_stringdata_interface)
{
    using sd = dci::qml::qmeta::Stringdata<a::b::c::I1<>>;

    checkOne<sd, introspection::typeName<a::b::c::I1<>>>();

    checkOne<sd, introspection::methodName<a::b::c::I1<>, 0>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 0, 0>>();

    checkOne<sd, introspection::methodName<a::b::c::I1<>, 1>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 1, 0>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 1, 1>>();

    checkOne<sd, introspection::methodName<a::b::c::I1<>, 2>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 2, 0>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 2, 1>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 2, 2>>();

    checkOne<sd, introspection::methodName<a::b::c::I1<>, 3>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 3, 0>>();

    checkOne<sd, introspection::methodName<a::b::c::I1<>, 4>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 4, 0>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 4, 1>>();

    checkOne<sd, introspection::methodName<a::b::c::I1<>, 5>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 5, 0>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 5, 1>>();
    checkOne<sd, introspection::methodParamName<a::b::c::I1<>, 5, 2>>();
}
