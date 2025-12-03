// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../collector.hpp"
#include <dci/utils/h2b.hpp>

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setMetaFile(std::string v)
    {
        auto p = fs::canonical(v);
        if(!fs::is_regular_file(p))
        {
            throw std::runtime_error{"bad meta file path: "+v+" ("+p.string()+")"};
        }

        _metaFile = p;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setVendorKey(std::string v)
    {
        if(v.size() != _vendorKey.size()*2 ||
            !dci::utils::h2b(v.data(), v.size(), _vendorKey.data()))
        {
            throw std::runtime_error{"bad vendor key value: "+v};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setStorageDir(std::string v)
    {
        fs::create_directories(v);
        auto p = fs::canonical(v);
        if(!fs::is_directory(p))
        {
            throw std::runtime_error{"bad resulting storage dir: "+v+" ("+p.string()+")"};
        }

        _storageDir = p;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setIgnoreSources(bool v)
    {
        _ignoreSources = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setIgnoreDebug4Targets(bool v)
    {
        _ignoreDebug4Targets = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setIgnoreDebug4Others(bool v)
    {
        _ignoreDebug4Others = v;
    }
}
