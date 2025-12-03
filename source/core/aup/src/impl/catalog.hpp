// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/bytes.hpp>
#include <dci/aup/catalog/object.hpp>

namespace dci::aup::impl::catalog
{
    class Enumerator;
}

namespace dci::aup::impl
{
    class Catalog final
    {
        Catalog(const Catalog&) = delete;
        Catalog(Catalog&&) = delete;

        void operator=(const Catalog&) = delete;
        void operator=(Catalog&&) = delete;

    public:
        Catalog();
        ~Catalog();

    public:
        void reset();

        void import(Catalog* from, bool(*filter)(const Oid& oid, const aup::catalog::ObjectPtr& object));

        uint32 dropOthersThan(const Set<Oid>& keep);

    public://весь индекс в блоб и обратно
        void deserialize(Bytes&& blob);
        Bytes serialize();

    public://перечисление
        Set<Oid> enumerate(aup::catalog::Object::Type type = aup::catalog::Object::Type::null);

    public://объектный ввод/вывод
        Oid put(aup::catalog::ObjectPtr&& object);
        bool has(const Oid& oid);
        aup::catalog::ObjectPtr get(const Oid& oid);
        void del(const Oid& oid);

    private:
        using ObjectsByOid = std::map<Oid, aup::catalog::ObjectPtr>;
        ObjectsByOid _objectsByOid;
    };
}
