// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "store.hpp"
#include "domain.hpp"
#include "path.hpp"

#include <mutex>

namespace dci::module::www::http::client::cookies
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Store::Store(const api::http::client::cookies::Store<>::Opposite& api, sbs::Owner& sol)
    {
        // in set(list<Entry>);
        api->set() += sol * [this](List<api::http::client::cookies::Entry>&& entries)
        {
            set(std::move(entries));
        };

        // in get(entry::Id) -> list<Entry>;
        api->get() += sol * [this](api::http::client::cookies::entry::Id&& entryId)
        {
            return cmt::readyFuture(get(std::move(entryId)));
        };

        // in del(entry::Id);
        api->del() += sol * [this](api::http::client::cookies::entry::Id&& entryId)
        {
            del(std::move(entryId));
        };

        // in expire(uint64 now, bool finalizeSession);
        api->expire() += sol * [this](uint64 now, bool finalizeSession)
        {
            expire(now, finalizeSession);
        };

        // in serialize() -> bytes;
        api->serialize() += sol * [this]()
        {
            return cmt::readyFuture(serialize());
        };

        // in deserialize(bytes) -> none;
        api->deserialize() += sol * [this](Bytes&& data)
        {
            if(ExceptionPtr error = deserialize(std::move(data)))
                return cmt::readyFuture<None>(error);
            return cmt::readyFuture(None{});
        };

        // in watchNotifies(store::Notifies::Opposite);
        api->watchNotifies() += sol * [this, &sol](api::http::client::cookies::store::Notifies<idl::interface::Side::opposite>&& watcher)
        {
            watcher.involvedChanged() += sol * [this, watcher](bool v)
            {
                if(!v)
                    _watchers.erase(watcher);
            };

            _watchers.emplace(std::move(watcher));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Store::~Store()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::set(api::http::client::cookies::Entry&& entry)
    {
        if(utils::dns::CanonicalizeResult::badInput == domain::canonicalize(entry.id.domain))
            return;

        path::canonicalize(entry.id.path, "/");

        for(api::http::client::cookies::Entry& old : _entries)
        {
            if(old.id == entry.id)
            {
                entry.creationTime = old.creationTime;
                add4Notify(Notify::changed, entry);
                old = std::move(entry);
                return;
            }
        }

        if(_entries.size() >= 3000)
            return;
        add4Notify(Notify::added, entry);
        _entries.emplace_back(std::move(entry));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::set(List<api::http::client::cookies::Entry>&& entries)
    {
        for(api::http::client::cookies::Entry& entry : entries)
            set(std::move(entry));
        entries.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool entryIdMatched(const api::http::client::cookies::entry::Id& target, const api::http::client::cookies::entry::Id& pattern)
    {
        if(!pattern.name.empty())
        {
            if(pattern.name != target.name)
                return false;
        }

        if(!pattern.domain.empty())
        {
            if(pattern.hostOnly)
            {
                if(pattern.domain != target.domain)
                    return false;
            }
            else
                if(!domain::matched(target.domain, pattern.domain))
                    return false;
        }

        if(!pattern.path.empty())
        {
            if(!path::matched(target.path, pattern.path))
                return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    List<api::http::client::cookies::Entry> Store::get(api::http::client::cookies::entry::Id&& pattern)
    {
        List<api::http::client::cookies::Entry> getted;

        if(!pattern.domain.empty())
        {
            switch(domain::canonicalize(pattern.domain))
            {
            case dci::utils::dns::CanonicalizeResult::ip:
                pattern.hostOnly = true;
                break;
            case dci::utils::dns::CanonicalizeResult::unneeded:
            case dci::utils::dns::CanonicalizeResult::ok:
                break;
            case dci::utils::dns::CanonicalizeResult::badInput:
            default:
                LOGD("failed to canonicalize domain: [" << pattern.domain << "]");
                return getted;
            }
        }

        if(!pattern.path.empty())
            path::canonicalize(pattern.path, "/");

        for(auto iter = _entries.begin(); iter != _entries.end(); ++iter)
        {
            if(entryIdMatched(iter->id, pattern))
                getted.emplace_back(*iter);
        }

        return getted;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::del(api::http::client::cookies::entry::Id&& pattern)
    {
        if(!pattern.domain.empty())
        {
            switch(domain::canonicalize(pattern.domain))
            {
            case dci::utils::dns::CanonicalizeResult::ip:
                pattern.hostOnly = true;
                break;
            case dci::utils::dns::CanonicalizeResult::unneeded:
            case dci::utils::dns::CanonicalizeResult::ok:
                break;
            case dci::utils::dns::CanonicalizeResult::badInput:
            default:
                LOGD("failed to canonicalize domain: [" << pattern.domain << "]");
                return;
            }
        }

        if(!pattern.path.empty())
            path::canonicalize(pattern.path, "/");

        for(auto iter = _entries.begin(); iter != _entries.end(); )
        {
            if(entryIdMatched(iter->id, pattern))
            {
                add4Notify(Notify::deleted, std::move(*iter));
                iter = _entries.erase(iter);
            }
            else
                ++iter;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::expire(uint64 now, bool finalizeSession)
    {
        for(auto iter = _entries.begin(); iter != _entries.end(); )
        {
            bool needFinalize = iter->persistent ?
                                    iter->expiryTime < now :
                                    finalizeSession;
            if(needFinalize)
            {
                add4Notify(Notify::deleted, std::move(*iter));
                iter = _entries.erase(iter);
            }
            else
                ++iter;
        }
    }

    namespace
    {
        using Magic = uint64;
        constexpr Magic magic = 0xf3b7a8030fcd14f4;

        using Check = Array<byte, 64>;
        Array<byte, 64> evalCheck(const Bytes& blob)
        {
            crypto::Blake2b hashier{};

            for(bytes::Cursor c{blob.begin()}; c.atEnd(); c.advanceChunks(1))
                hashier.add(c.continuousData(), c.continuousDataSize());

            Check result{};
            hashier.finish(result.data());

            return result;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Store::serialize()
    {
        Bytes blob;

        stiac::serialization::Arch{blob.begin()} << magic << _entries;

        Check check = evalCheck(blob);
        blob.end().write(check.data(), check.size());

        return blob;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionPtr Store::deserialize(Bytes&& blob)
    {
        if(blob.size() < 8 + 64)//magic + check
            return exception::buildInstance<api::http::client::cookies::store::deserialization::TooSmall>();

        Check check{};
        {
            bytes::Alter a{blob.end()};
            a.advance(-int32{check.size()});
            a.removeTo(check.data(), check.size());
        }

        if(check != evalCheck(blob))
            return exception::buildInstance<api::http::client::cookies::store::deserialization::BadCheck>();

        stiac::serialization::Arch arch{blob.begin()};
        Magic blobMagic;
        arch >> blobMagic;
        if(magic != blobMagic)
            return exception::buildInstance<api::http::client::cookies::store::deserialization::BadMagic>();

        List<api::http::client::cookies::Entry> entries;
        try
        {
            arch >> entries;
        }
        catch(...)
        {
            return exception::buildInstance<api::http::client::cookies::store::deserialization::BadContent>(std::current_exception());
        }

        set(std::move(entries));
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::flushNotifies()
    {
        if(_notifies.empty())
            return;

        List<Notify> notifies;
        notifies.swap(_notifies);

        if(_watchers.empty())
            return;

        Notify::Kind kind{};
        List<api::http::client::cookies::Entry> added;
        List<api::http::client::cookies::Entry> changed;
        List<api::http::client::cookies::Entry> deleted;

        auto doFlush = [&]()
        {
            auto iter{_watchers.begin()};
            auto end{_watchers.end()};
            dbgAssert(iter != end);
            auto last{--end};

            for(; iter!=last; ++iter)
            {
                if(!added  .empty()) (*iter)->added  (added  );
                if(!changed.empty()) (*iter)->changed(changed);
                if(!deleted.empty()) (*iter)->deleted(deleted);
            }

            if(!added  .empty()) (*last)->added  (std::exchange(added  , {}));
            if(!changed.empty()) (*last)->changed(std::exchange(changed, {}));
            if(!deleted.empty()) (*last)->deleted(std::exchange(deleted, {}));
        };

        auto doOne = [&](Notify&& notify)
        {
            if(kind != notify._kind)
                doFlush();

            kind = notify._kind;
            List<api::http::client::cookies::Entry>* list{};
            switch(kind)
            {
            case Notify::added:   list = &added; break;
            case Notify::changed: list = &changed; break;
            case Notify::deleted: list = &deleted; break;
            }

            list->emplace_back(std::move(notify._entry));
        };

        for(Notify& notify : notifies)
            doOne(std::move(notify));
        doFlush();
    }
}
