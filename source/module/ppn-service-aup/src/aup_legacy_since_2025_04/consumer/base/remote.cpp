// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "remote.hpp"
#include "downloader.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Remote::Remote(remote::Api&& api)
        : _api{std::move(api)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Remote::~Remote()
    {
        uninvolved();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner& Remote::sol()
    {
        return _sbsOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const remote::Api& Remote::api()
    {
        return _api;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::uninvolved()
    {
        _sbsOwner.flush();
        for(Downloader* d : std::exchange(_downloaders, {}))
        {
            d->uninvolve(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::involve(Downloader* d)
    {
        _downloaders.insert(d);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::uninvolve(Downloader* d)
    {
        _downloaders.erase(d);
    }
}
