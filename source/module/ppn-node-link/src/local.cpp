// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "local.hpp"
#include "handshake.hpp"
#include "remote.hpp"

namespace dci::module::ppn::node::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Local::Local(host::Manager* hostManager)
        : api::Local<>::Opposite{idl::interface::Initializer{}}
        , _hostManager(hostManager)
        , _featureService(idl::interface::Initializer{})
    {
        //in id() -> Id;
        _featureService->id() += serviceSol() * [this]
        {
            return cmt::readyFuture(_id);
        };

        //in addPayload(Payload);
        _featureService->addPayload() += serviceSol() * [this](api::feature::Payload<> payload)
        {
            PayloadState& dps = _payloads[payload];
            payload.involvedChanged() += dps._sbsOwner * [this, payload=payload.weak()](bool v)
            {
                if(!v)
                {
                    _payloads.erase(payload);
                    reindexPayload();
                }
            };

            auto payloadIdsFetcher = [this, payload=payload.weak()]() mutable
            {
                api::feature::Payload<>{payload}->ids().then() += _payloads[payload]._sbsOwner * [this, payload](auto ids)
                {
                    if(ids.resolvedValue())
                    {
                        _payloads[payload]._ids = ids.detachValue();
                    }
                    else
                    {
                        _payloads[payload]._ids.clear();
                    }

                    reindexPayload();
                };
            };

            payload->idsChanged() += dps._sbsOwner * payloadIdsFetcher;
            payloadIdsFetcher();
        };

        //in setFeatures(list<Feature> features);
        methods()->setFeatures() += serviceSol() * [this](List<api::Feature<>>&& features)
        {
            for(api::Feature<>& f : features)
            {
                f->setup(_featureService);
            }
        };

        methods()->setKey() += serviceSol() * [this](const api::Key& key)
        {
            if(api::Key{} != _key)
            {
                //already specified
                return;
            }

            _key = key;

            dbgAssert(32 == _id.size());
            dbgAssert(32 == _key.size());
            crypto::curve25519::basepoint(_key.data(), _id.data());
            crypto::blake2b(_id.data(), _id.size(), _id.data(), _id.size());
        };

        methods()->id() += serviceSol() * [this]
        {
            return cmt::readyFuture(_id);
        };

        methods()->joinByConnect() += serviceSol() *[this](apit::Channel<>&& ch)
        {
            HandshakePtr handshake(new Handshake(this, std::move(ch), true));
            Handshake* hp = handshake.get();
            _handshakes.insert(std::move(handshake));

            cmt::Future<api::Remote<>> res = hp->completion();
            hp->start();
            return res;
        };

        methods()->joinByAccept() += serviceSol() * [this](apit::Channel<>&& ch)
        {
            HandshakePtr handshake(new Handshake(this, std::move(ch), false));
            Handshake* hp = handshake.get();
            _handshakes.insert(std::move(handshake));

            cmt::Future<api::Remote<>> res = hp->completion();
            hp->start();
            return res;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Local::~Local()
    {
        serviceSol().flush();

        _handshakes.clear();
        _remotes.clear();

        _payloadIds.clear();
        _payloads.clear();
        _payloadsByILid.clear();

        _featureService.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const api::Key& Local::key() const
    {
        return _key;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Local::handshakeCompleted(Handshake* instance, remote::State&& state, cmt::Promise<api::Remote<>> completion)
    {
        RemotePtr remote = Remote::create(this, std::move(state));
        Remote* rp = remote.get();
        _remotes.insert(std::move(remote));

        HandshakePtr hptr{instance};
        _handshakes.erase(hptr);
        (void)hptr.release();

        rp->start();

        completion.resolveValue(*rp);

        if(rp->state()._byConnect)
        {
            _featureService->joinedByConnect(rp->state()._id, *rp);
        }
        else
        {
            _featureService->joinedByAccept(rp->state()._id, *rp);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Local::handshakeFailed(Handshake* instance)
    {
        HandshakePtr hptr{instance};
        _handshakes.erase(hptr);
        (void)hptr.release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Local::remoteClosed(Remote* instance)
    {
        RemotePtr rptr{instance};
        _remotes.erase(rptr);
        (void)rptr.release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<idl::ILid>& Local::payloadIds()
    {
        return _payloadIds;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Local::getPayloadInstance(Remote* r, idl::ILid ilid)
    {
        auto iter = _payloadsByILid.find(ilid);
        if(_payloadsByILid.end() == iter)
        {
            return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::BadPayloadIdRequested>());
        }

        return iter->second->getInstance(r->state()._id, r->opposite(), ilid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Local::reindexPayload()
    {
        Set<idl::ILid> payloadIds;
        _payloadsByILid.clear();

        for(auto&[i, state] : _payloads)
        {
            for(auto id : state._ids)
            {
                payloadIds.insert(id);
                _payloadsByILid[id] = i;
            }
        }

        if(_payloadIds != payloadIds)
        {
            _payloadIds.swap(payloadIds);

            for(const RemotePtr& r : _remotes)
            {
                r->payloadOutIdsChanged();
            }
        }
    }
}
