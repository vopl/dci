// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
