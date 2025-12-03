// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "collector.hpp"

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Collector::Collector()
        : _globalMeta{nullptr, "global"}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Collector::~Collector()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::run()
    {
        loadMeta();

        _aupStorage.reset(_storageDir.string());

        auto prevCatalogBlob = _aupStorage.get("catalog");
        if(prevCatalogBlob)
        {
            _aupCatalog.deserialize(std::move(*prevCatalogBlob));
        }

        processFiles();
        fixRelease();

        _aupStorage.put("catalog", _aupCatalog.serialize());
    }
}
