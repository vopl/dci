// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::tls
{
    class Channel
    {
    public:
        struct Settings
        {
            std::vector<unsigned char> _encodedAlpnProtos;
            String _servername;
        };

    public:
        Channel(SSL_CTX* sslCtx, const Settings& settings, const api::tls::Channel<>::Opposite& user, api::stream::Channel<>&& peer);
        virtual ~Channel();

        sbs::Owner& sol();

        virtual cmt::Future<> handshake();

    public:
        std::tuple<bool, uint32> bioAskWrite(const void* data, uint32 size);
        std::tuple<bool, uint32> bioAskRead(void* data, uint32 maxSize);
        bool bioAskEof();

    private:
        void fsmTick();
        void cleanup(bool peerShutdown, ExceptionPtr userFail);

    protected:
        sbs::Owner                          _solExternal;
        sbs::Owner                          _sol;

        Settings                            _settings;

        api::tls::Channel<>::Opposite       _user;
        Bytes                               _userWrote;
        bool                                _userReceiveStarted{};
        bool                                _userClosed{};

        api::stream::Channel<>              _peer;
        Bytes                               _peerReceived;
        bool                                _peerShuttedDown{};
        bool                                _peerClosed{};
        ExceptionPtr                        _peerFail{};
        std::unique_ptr<BIO, void(*)(BIO*)> _peerBio;

        std::unique_ptr<SSL, void(*)(SSL*)> _ssl;
        enum class SslState
        {
            handshake,
            work,
            shutdown,
            done
        };
        SslState _sslState{};
        String _alpnProtoSelected;

        cmt::Promise<>                      _handshakePromise;
    };
}
