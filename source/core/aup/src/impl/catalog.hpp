/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
