// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/aup/catalog/release.hpp>
#include <dci/aup/catalog/file.hpp>
#include <dci/aup/catalog/object.hpp>
#include <dci/aup/catalog/unit.hpp>
#include <dci/aup/catalog/identify.hpp>
#include <dci/aup/exception.hpp>
#include <dci/crypto/blake3.hpp>
#include <dci/stiac/serialization.hpp>

#include "../impl/catalog/enumerateObjectFields.hpp"

namespace dci::aup::catalog
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        struct OidMaker
        {
            crypto::Blake3 _hashier{32};

            void write(const void* data, uint32 size)
            {
                _hashier.add(data, size);
            }

            OidMaker& operator<<(auto&& v)
            {
                using stiac::serialization::save;

                save(*this, std::forward<decltype(v)>(v));
                return *this;
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid identify(const Bytes& blob)
    {
        OidMaker arch;

        bytes::Cursor c{blob.begin()};

        while(!c.atEnd())
        {
            arch.write(c.continuousData(), c.continuousDataSize());
            c.advanceChunks(1);
        }

        Oid res;
        dbgAssert(res.size() == arch._hashier.digestSize());
        arch._hashier.finish(res.data());

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid identify(std::FILE* f)
    {
        OidMaker arch;

        rewind(f);
        char buf[1024];

        for(;;)
        {
            std::size_t s = fread(buf, 1, sizeof(buf), f);
            if(!s)
            {
                break;
            }

            arch.write(buf, static_cast<uint32>(s));

            if(s != sizeof(buf))
            {
                break;
            }
        }

        Oid res;
        dbgAssert(res.size() == arch._hashier.digestSize());
        arch._hashier.finish(res.data());

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid identify(const aup::catalog::Object* object)
    {
        OidMaker arch;
        arch << object->type();

        impl::catalog::enumerateObjectFields(object, [&](const auto& fld)
        {
            arch << fld;
        });

        Oid res;
        dbgAssert(res.size() == arch._hashier.digestSize());
        arch._hashier.finish(res.data());

        return res;
    }
}
