// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/aup/implMetaInfo.hpp>
#include "api.hpp"
#include "oid.hpp"
#include "applier/task.hpp"
#include "applier/result.hpp"
#include "catalog/file.hpp"

namespace dci::aup
{
    class Catalog;
    class Storage;

    class API_DCI_AUP Applier
        : public himpl::FaceLayout<Applier, impl::Applier>
    {
        Applier(const Applier&) = delete;
        Applier(Applier&&) = delete;

        void operator=(const Applier&) = delete;
        void operator=(Applier&&) = delete;

    public:
        Applier();
        ~Applier();

    public:
        void addCatalog(Catalog* c);
        void addStorage(Storage* s);
        void addRoot(const Oid& oid, const Set<catalog::File::Kind>& fileKinds);

    public:
        applier::Result process(const String& place, applier::Task task = applier::tNull);
    };
}
