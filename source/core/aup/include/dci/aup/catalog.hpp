// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/aup/implMetaInfo.hpp>
#include "api.hpp"
#include "oid.hpp"
#include "catalog/object.hpp"
#include <dci/bytes.hpp>

namespace dci::aup
{
    class API_DCI_AUP Catalog
        : public himpl::FaceLayout<Catalog, impl::Catalog>
    {
        Catalog(const Catalog&) = delete;
        Catalog(Catalog&&) = delete;

        void operator=(const Catalog&) = delete;
        void operator=(Catalog&&) = delete;

    public:
        Catalog();
        ~Catalog();

    public://весь индекс в блоб и обратно
        void deserialize(Bytes&& blob);
        Bytes serialize();

    public://перечисление
        Set<Oid> enumerate(catalog::Object::Type type = catalog::Object::Type::null);

    public://объектный ввод/вывод
        Oid put(catalog::ObjectPtr&& object);
        bool has(const Oid& oid);
        catalog::ObjectPtr get(const Oid& oid);
        void del(const Oid& oid);
    };
}
