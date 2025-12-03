// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include "../oid.hpp"
#include <optional>
#include <dci/bytes.hpp>

namespace dci::aup::instance::io
{
    API_DCI_AUP bool instanceInitialized();

    API_DCI_AUP const Set<Oid>& allReleases();

    API_DCI_AUP const Set<Oid>& targetMostReleases();
    API_DCI_AUP const Set<Oid>& targetCatalogIncomplete();
    API_DCI_AUP const Set<Oid>& targetCatalogComplete();
    API_DCI_AUP const Set<Oid>& targetStorageIncomplete();
    API_DCI_AUP const Set<Oid>& targetStorageComplete();

    API_DCI_AUP const Set<Oid>& bufferMostReleases();
    API_DCI_AUP const Set<Oid>& bufferCatalogIncomplete();
    API_DCI_AUP const Set<Oid>& bufferCatalogComplete();
    API_DCI_AUP const Set<Oid>& bufferStorageIncomplete();
    API_DCI_AUP const Set<Oid>& bufferStorageComplete();

    API_DCI_AUP bool hasCatalogObject(const Oid& oid);
    API_DCI_AUP bool hasStorageObject(const Oid& oid);

    API_DCI_AUP std::optional<Bytes> getCatalogObject(const Oid& oid);
    API_DCI_AUP std::optional<Bytes> getStorageObject(const Oid& oid, uint32 from=0, uint32 to=~uint32{});

    enum class PutObjectResult
    {
        ok,
        corrupted,
        unwanted
    };

    API_DCI_AUP PutObjectResult putCatalogObject(const Oid& oid, Bytes&& blob);
    API_DCI_AUP PutObjectResult putStorageObject(const Oid& oid, Bytes&& blob);
    API_DCI_AUP PutObjectResult putStorageObject(const Oid& oid, std::FILE* f);
}
