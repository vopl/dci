// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "catalog.hpp"
#include "catalog/enumerateObjectFields.hpp"
#include "catalog/serializeObject.hpp"
#include "catalog/deserializeObject.hpp"
#include <dci/stiac/serialization.hpp>
#include <dci/aup/exception.hpp>
#include <dci/aup/catalog/release.hpp>
#include <dci/aup/catalog/file.hpp>
#include <dci/aup/catalog/object.hpp>
#include <dci/aup/catalog/unit.hpp>
#include <dci/aup/catalog/identify.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/logger.hpp>

namespace dci::aup::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::Catalog()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::~Catalog()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::reset()
    {
        _objectsByOid.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::import(Catalog* from, bool(*filter)(const Oid& oid, const aup::catalog::ObjectPtr& object))
    {
        for(auto&[oid, obj] : from->_objectsByOid)
        {
            if(!filter || filter(oid, obj))
            {
                //LOGI("imported catalog entry: "<<utils::b2h(oid));
                _objectsByOid.emplace(oid, std::move(obj));
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Catalog::dropOthersThan(const Set<Oid>& keep)
    {
        uint32 res{};

        for(ObjectsByOid::iterator iter{_objectsByOid.begin()}; iter!=_objectsByOid.end(); )
        {
            const Oid& oid = iter->first;
            if(keep.count(oid))
            {
                ++iter;
            }
            else
            {
                iter = _objectsByOid.erase(iter);
                ++res;
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::deserialize(Bytes&& blob_)
    {
        Bytes blob{std::move(blob_)};

        if(blob.size() < Oid{}.size())//check
        {
            throw aup::Exception{"low data for deserialize catalog"};
        }

        {
            Oid check;
            {
                bytes::Alter a{blob.end()};
                a.advance(-int32{check.size()});
                a.removeTo(check.data(), check.size());
            }

            if(check != aup::catalog::identify(blob))
            {
                throw aup::Exception{"corrupted data for deserialize catalog"};
            }
        }

        std::vector<aup::catalog::ObjectPtr> objects;

        {
            stiac::serialization::Arch arch{blob.begin()};

            uint64 magic;
            arch >> magic;

            if(uint64{0xe32206afe2bced65} != magic)
            {
                throw aup::Exception{"unknown magic for deserialize catalog: "+std::to_string(magic)};
            }

            while(!arch.atEnd())
            {
                auto o = catalog::deserializeObject(arch);
                if(o)
                {
                    objects.push_back(std::move(o));
                }
            }
        }

        for(aup::catalog::ObjectPtr& o : objects)
        {
            put(std::move(o));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Catalog::serialize()
    {
        Bytes blob;

        {
            stiac::serialization::Arch arch{blob.begin()};

            //magic
            arch << uint64{0xe32206afe2bced65};

            //content
            for(const auto& [oid, object] : _objectsByOid)
            {
                (void)oid;
                catalog::serializeObject(std::move(object), arch);
            }

            //check
            blob.end().write(aup::catalog::identify(blob));
        }

        return blob;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<Oid> Catalog::enumerate(aup::catalog::Object::Type type)
    {
        Set<Oid> res;

        for(ObjectsByOid::iterator iter{_objectsByOid.begin()}; iter!=_objectsByOid.end(); ++iter)
        {
            const aup::catalog::ObjectPtr& object = iter->second;
            if(aup::catalog::Object::Type::null == type || object->type() == type)
            {
                const Oid& oid = iter->first;
                res.insert(oid);
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Catalog::put(aup::catalog::ObjectPtr&& object)
    {
        Oid oid = identify(object.get());

        _objectsByOid.insert_or_assign(oid, std::move(object));

        return oid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Catalog::has(const Oid& oid)
    {
        return _objectsByOid.count(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    aup::catalog::ObjectPtr Catalog::get(const Oid& oid)
    {
        auto iter = _objectsByOid.find(oid);
        if(_objectsByOid.end() == iter)
        {
            return {};
        }

        aup::catalog::Object* o = iter->second.get();
        switch(o->type())
        {
        case aup::catalog::Object::Type::file:
            {
                return std::make_unique<aup::catalog::File>(*static_cast<aup::catalog::File*>(o));
            }
            break;
        case aup::catalog::Object::Type::unit:
            {
                return std::make_unique<aup::catalog::Unit>(*static_cast<aup::catalog::Unit*>(o));
            }
            break;
        case aup::catalog::Object::Type::release:
            {
                return std::make_unique<aup::catalog::Release>(*static_cast<aup::catalog::Release*>(o));
            }
            break;
        default:
            dbgWarn("bad object type");
            throw aup::Exception{"bad object type requested"};
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::del(const Oid& oid)
    {
        auto iter = _objectsByOid.find(oid);
        if(_objectsByOid.end() == iter)
        {
            return;
        }

        _objectsByOid.erase(iter);
    }
}
