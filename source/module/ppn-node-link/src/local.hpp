// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "remote/state.hpp"

namespace dci::module::ppn::node::link
{
    class Handshake;
    using HandshakePtr = std::unique_ptr<Handshake>;

    class Remote;
    using RemotePtr = std::unique_ptr<Remote>;

    class Local
        : public api::Local<>::Opposite
        , public host::module::ServiceBase<Local>
    {
    public:
        Local(host::Manager* hostManager);
        ~Local();

    public:
        template <class Interface>
        cmt::Future<Interface> createServiceFromHost() const;

        const api::Key& key() const;

        void handshakeCompleted(Handshake* instance, remote::State&& state, cmt::Promise<api::Remote<>> completion);
        void handshakeFailed(Handshake* instance);
        void remoteClosed(Remote* instance);

    public:
        const Set<idl::ILid>& payloadIds();
        cmt::Future<idl::Interface> getPayloadInstance(Remote* r, idl::ILid ilid);

    private:
        void reindexPayload();

    private:
        host::Manager * _hostManager;

        api::Key    _key {};
        api::Id     _id  {};

        api::feature::Service<>::Opposite   _featureService;

    private:
        struct PayloadState
        {
            Set<idl::ILid>  _ids;
            sbs::Owner      _sbsOwner;
        };

        Set<idl::ILid>                              _payloadIds;
        Map<api::feature::Payload<>, PayloadState>  _payloads;
        Map<idl::ILid, api::feature::Payload<>>     _payloadsByILid;

    private:
        std::set<HandshakePtr>  _handshakes;
        std::set<RemotePtr>     _remotes;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Interface>
    cmt::Future<Interface> Local::createServiceFromHost() const
    {
        return _hostManager->template createService<Interface>();
    }
}
