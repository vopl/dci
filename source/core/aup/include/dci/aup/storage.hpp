/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/himpl.hpp>
#include <dci/aup/implMetaInfo.hpp>
#include "api.hpp"
#include "oid.hpp"
#include <dci/bytes.hpp>
#include <optional>

namespace dci::aup
{
    class API_DCI_AUP Storage
        : public himpl::FaceLayout<Storage, impl::Storage>
    {
        Storage(const Storage&) = delete;
        Storage(Storage&&) = delete;

        void operator=(const Storage&) = delete;
        void operator=(Storage&&) = delete;

    public:
        Storage();
        ~Storage();

        /* ошибки
         *
         * в случае ошибок нижнего слоя (файловая система)
         *      при взведеном флаге autoFixIfCan будет предпринята попытка исправить ситуацию, даже если для исправления потребуется утерять информацию
         *      если исправить не удалось или флажок вообще не взведен - будет бросаться исключение
         */

    public:
        void reset(const std::string& place, bool autoFixIfCan=true);

    public:
        Set<Oid> enumerate();

    public:
        void put(const std::string& localPath, Bytes&& blob);
        bool has(const std::string& localPath);
        std::optional<Bytes> get(const std::string& localPath, uint32 from=0, uint32 to=~uint32{0});
        bool del(const std::string& localPath);

        void put(const Oid& oid, Bytes&& blob);
        bool has(const Oid& oid);
        std::optional<Bytes> get(const Oid& oid, uint32 from=0, uint32 to=~uint32{0});
        bool del(const Oid& oid);

        void delAll(bool andPlaceDirectory = true);
    };
}
