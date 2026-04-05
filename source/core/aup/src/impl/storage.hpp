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
#include <dci/aup/oid.hpp>
#include <optional>
#include <filesystem>

namespace dci::aup::impl
{
    class Storage final
    {
        Storage(const Storage&) = delete;
        Storage(Storage&&) = delete;

        void operator=(const Storage&) = delete;
        void operator=(Storage&&) = delete;

    public:
        Storage();
        ~Storage();

        void reset();
        void reset(const std::string& place, bool autoFixIfCan=true);

        void import(Storage* from);

        uint32 dropOthersThan(const Set<Oid>& keep);

    public:
        Set<Oid> enumerate();

    public:
        void put(const std::string& localPath, Bytes&& blob);
        void put(const std::string& localPath, std::FILE* f);
        bool has(const std::string& localPath);
        std::optional<Bytes> get(const std::string& localPath, uint32 offset=0, uint32 size=~uint32{0});
        bool del(const std::string& localPath);

        void put(const Oid& oid, Bytes&& blob);
        void put(const Oid& oid, std::FILE* f);
        bool has(const Oid& oid);
        std::optional<Bytes> get(const Oid& oid, uint32 offset=0, uint32 size=~uint32{0});
        bool del(const Oid& oid);

        void delAll(bool andPlaceDirectory);

    private:
        void put_(const std::filesystem::path& path, Bytes&& blob);
        void put_(const std::filesystem::path& path, std::FILE* f);
        bool has_(const std::filesystem::path& path);
        std::optional<Bytes> get_(const std::filesystem::path& path, uint32 offset=0, uint32 size=~uint32{0});
        bool del_(const std::filesystem::path& path);

        std::filesystem::path filePath(const std::string& localPath);
        std::filesystem::path filePath(const Oid& oid);

    private:
        std::filesystem::path _place;
        bool _autoFixIfCan{true};
    };
}
