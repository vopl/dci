// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "signBuilder.hpp"
#include <dci/utils/endian.hpp>
#include <dci/utils/dbg.hpp>
#include <cstring>
#include <string_view>

namespace dci::idl::im
{
    using namespace dci::utils::endian;
    using namespace std::literals;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignBuilder::SignBuilder()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignBuilder::SignBuilder(const SignBuilder& other)
        : _hashier{other._hashier}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignBuilder::~SignBuilder()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignBuilder& SignBuilder::operator=(const SignBuilder& other)
    {
        _hashier = other._hashier;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(const Sign& v)
    {
        addImpl("sign"sv, v.data(), v._size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(const Full& v)
    {
        addImpl("full"sv, v.data(), v.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(const std::string& v)
    {
        addImpl("string"sv, v.data(), v.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::string_view v)
    {
        addImpl("string"sv, v.data(), v.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(const char* csz)
    {
        addImpl("string"sv, csz, strlen(csz));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(bool v)
    {
        v = !!v;
        addImpl("bool"sv, &v, sizeof(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::uint8_t v)
    {
        addImpl("ui8"sv, &v, sizeof(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::uint16_t v)
    {
        auto buf = n2b(v);
        addImpl("ui16"sv, &buf, sizeof(buf));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::uint32_t v)
    {
        auto buf = n2b(v);
        addImpl("ui32"sv, &buf, sizeof(buf));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::uint64_t v)
    {
        auto buf = n2b(v);
        addImpl("ui64"sv, &buf, sizeof(buf));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::int8_t v)
    {
        addImpl("i8"sv, &v, sizeof(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::int16_t v)
    {
        auto buf = n2b(v);
        addImpl("i16"sv, &buf, sizeof(buf));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::int32_t v)
    {
        auto buf = n2b(v);
        addImpl("i32"sv, &buf, sizeof(buf));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::add(std::int64_t v)
    {
        auto buf = n2b(v);
        addImpl("i64"sv, &buf, sizeof(buf));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignBuilder::Full SignBuilder::finish()
    {
        Full res{};

        dbgAssert(res.size() == _hashier.digestSize());
        _hashier.finish(res.data());

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignBuilder::addImpl(std::string_view tag, const void* data, std::size_t len)
    {
        _hashier.add(tag.data(), tag.size());
        _hashier.barrier();

        _hashier.add(data, len);
        _hashier.barrier();
    }
}
