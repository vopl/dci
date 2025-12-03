// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "impl/bytes.hpp"
#include <dci/bytes.hpp>

namespace dci
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes()
        : himpl::FaceLayout<Bytes, impl::Bytes>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(const Bytes& from)
        : himpl::FaceLayout<Bytes, impl::Bytes>(himpl::face2Impl(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(Bytes&& from)
        : himpl::FaceLayout<Bytes, impl::Bytes>(himpl::face2Impl(std::move(from)))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(bytes::Chunk* first, bytes::Chunk* last, uint32 size)
        : himpl::FaceLayout<Bytes, impl::Bytes>(first, last, size)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(const void* data, uint32 size)
        : himpl::FaceLayout<Bytes, impl::Bytes>(data, size)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(bytes::details::CszWrapper cszWrapper)
        : himpl::FaceLayout<Bytes, impl::Bytes>(cszWrapper._csz)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::~Bytes()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes& Bytes::operator=(const Bytes& from)
    {
        return himpl::impl2Face<Bytes>(impl()=himpl::face2Impl(from));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes& Bytes::operator=(Bytes&& from)
    {
        return himpl::impl2Face<Bytes>(impl()=himpl::face2Impl(std::move(from)));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator==(const Bytes& with) const
    {
        return impl() == himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator!=(const Bytes& with) const
    {
        return impl() != himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator<(const Bytes& with) const
    {
        return impl() < himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator>(const Bytes& with) const
    {
        return impl() > himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator<=(const Bytes& with) const
    {
        return impl() <= himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator>=(const Bytes& with) const
    {
        return impl() >= himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Bytes::operator<=>(const Bytes& with) const
    {
        return impl() <=> himpl::face2Impl(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::empty() const
    {
        return impl().empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Bytes::size() const
    {
        return impl().size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Bytes::toString() const
    {
        return impl().toString();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Bytes::toHex() const
    {
        return impl().toHex();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Bytes::clear()
    {
        return impl().clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Alter Bytes::begin()
    {
        return himpl::impl2Face<bytes::Alter>(impl().begin());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Cursor Bytes::begin() const
    {
        return himpl::impl2Face<bytes::Cursor>(impl().begin());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Cursor Bytes::cbegin() const
    {
        return himpl::impl2Face<bytes::Cursor>(impl().cbegin());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Alter Bytes::end()
    {
        return himpl::impl2Face<bytes::Alter>(impl().end());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Cursor Bytes::end() const
    {
        return himpl::impl2Face<bytes::Cursor>(impl().end());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bytes::Cursor Bytes::cend() const
    {
        return himpl::impl2Face<bytes::Cursor>(impl().end());
    }
}
