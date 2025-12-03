// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "quota.hpp"
#include "downloader.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Quota::Quota(std::size_t slots_)
        : _slots{slots_}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Quota::~Quota()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Quota::clear()
    {
        _ready.clear();
        _inprogress.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Quota::setSlots(std::size_t v)
    {
        _slots = v;
        update();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Quota::ready(Downloader* downloader)
    {
        _ready.emplace(downloader);
        _inprogress.erase(downloader);
        update();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Quota::done(Downloader* downloader)
    {
        _ready.erase(downloader);
        _inprogress.erase(downloader);
        update();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Quota::update()
    {
        while(_inprogress.size() < _slots && !_ready.empty())
        {
            Downloader* d = *_ready.begin();
            _ready.erase(_ready.begin());
            _inprogress.insert(d);
            d->canWork();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Quota::QueueCmp::operator()(Downloader*a, Downloader*b) const
    {
        return std::tuple{a->priority(), a} > std::tuple{b->priority(), b};
    }
}
