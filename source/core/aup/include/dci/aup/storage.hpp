// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
