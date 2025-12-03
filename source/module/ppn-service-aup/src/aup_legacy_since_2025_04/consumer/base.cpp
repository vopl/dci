// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "base.hpp"
#include "../../aup.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    using namespace base;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::Base(std::string_view name, Quota* quota)
        : _name{name}
        , _quota{quota}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::~Base()
    {
        _sbsOwner.flush();
        _downloaders.clear();
        _remotes.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Remote* Base::involve(remote::Api&& remoteApi)
    {
        remote::Ptr remote = std::make_unique<Remote>(std::move(remoteApi));
        Remote* raw = remote.get();
        _remotes.emplace(raw, std::move(remote));

        raw->api().involvedChanged() += raw->sol() * [=,this](bool b) mutable
        {
            if(!b)
            {
                auto rn = _remotes.extract(raw);
                raw->uninvolved();
            }
        };

        for(auto&[oid, d] : _downloaders)
        {
            d.involve(raw);
            raw->involve(&d);
        }

        return raw;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Base::addIncomplete(const Oid& oid, int priority)
    {
        auto entry = _downloaders.try_emplace(
                         oid,
                         this, oid, priority);

        if(entry.second)
        {
            for(auto&[r,rp] : _remotes)
            {
                entry.first->second.involve(r);
                r->involve(&entry.first->second);
            }

            return true;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    base::Quota& Base::quota()
    {
        return *_quota;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Base::ready(base::Downloader* d, RecvBuffer& recvBuffer)
    {
        try
        {
            return onComplete(d->oid(), recvBuffer);
        }
        catch(...)
        {
            LOGW(_name<<": blob placing failure: "<<exception::toString(std::current_exception()));
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::done(base::Downloader* d)
    {
        _downloaders.erase(d->oid());
    }
}
