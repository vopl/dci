// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "dht/storage.hpp"

namespace dci::module::ppn::service
{
    class Dht
        : public idl::gen::ppn::service::Dht<>::Opposite
        , public host::module::ServiceBase<Dht>
    {
    public:
        Dht();
        ~Dht();

    private:
        void updateReadyStatus();

    private:
        cmt::Future<None> onLocalPut(const api::Key& key, api::Value&& value, api::ProgressListner<>&& pl);
        cmt::Future<Set<api::Value>> onLocalGet(const api::Key& key, uint32 limit, api::ProgressListner<>&& pl);

    private:
        struct Remote;
        const Remote* moreClosest(const api::Key& key);

    private:
        auto awaitReady(auto&& f);

    private:
        cmt::task::Owner _tol;
    private:
        link::Id        _localId;
        bool            _localIdSetted = false;
        bool            _nodeStarted = false;
        cmt::Event      _ready;

        dht::Storage    _storage;
        poll::Timer     _storageCleaner
        {
            std::chrono::minutes{10},
            true,
            [this]{_storage.dropOld();},
            &_tol
        };

    private:
        api::internal::Peer<>::Opposite _local{idl::interface::Initializer{}};

    private:
        struct Remote
        {
            link::Id _id;
            api::internal::Peer<> _api;

            Remote(auto&& id, auto&& api)
                : _id{std::forward<decltype(id)>(id)}
                , _api{std::forward<decltype(api)>(api)}
            {
            }

        };
        using RemoteById = boost::multi_index::member<Remote, link::Id, &Remote::_id>;
        using RemoteByApi = boost::multi_index::member<Remote, api::internal::Peer<>, &Remote::_api>;

        struct IdCmp
        {
            bool operator()(const link::Id& a, const link::Id& b) const
            {
                constexpr uint32 size = link::Id{}.size();
                for(uint32 idx{size-1}; idx < size; --idx)
                {
                    if(a[idx] == b[idx])
                    {
                        continue;
                    }

                    return a[idx] < b[idx];
                }

                return false;
            }
        };

        using Remotes = boost::multi_index::multi_index_container
        <
            Remote,
            boost::multi_index::indexed_by
            <
                boost::multi_index::ordered_non_unique <boost::multi_index::tag<RemoteById>, RemoteById, IdCmp>,
                boost::multi_index::ordered_unique <boost::multi_index::tag<RemoteByApi>, RemoteByApi>
            >
        >;

        Remotes _remotes;
    };
}
