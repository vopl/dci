// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "secret.hpp"

namespace dci::module::stiac
{
    class Protocol;
}

namespace dci::module::stiac::stages::in
{
    class Ciphering;
}

namespace dci::module::stiac::stages::out
{
   class Ciphering;
}

namespace dci::module::stiac::crypto
{
    class Symmetric;

    class Handshake
        : public mm::heap::Allocable<Handshake>
    {
    public:
        static constexpr uint32 _publicKeySize = 32;
        static constexpr uint32 _privateKeySize = 32;
        static constexpr uint32 _symmetricKeySize = 32;

        enum class MessageType : uint8
        {
            payloadChunk        = 0,
            payloadLastChunk    = 1,
            protocolMarker      = 2,

            ekey                = 3,
            skey                = 4,
            keyApplied          = 5,

            maxValue            = 15,
            fakeNull            = 16,
        };

    public:
        Handshake(
                Protocol* protocol,
                stages::in::Ciphering* inCiphering,
                stages::out::Ciphering* outCiphering);

        ~Handshake();

    public:
        void setAuth(const Bytes& prologue, const apip::PrivateKey& local);
        apip::PublicKey remoteAuth();

        void start();

    public:

        void someInputMessageDeciphered();

        void outputHasDisallowedPayload();
        void inputComing(MessageType messageType, bytes::Alter& data);

        void outputTraffic(uint32 size);
        void inputTraffic(uint32 size);

    private:
        void growLocalKey();
        void cropLocalKey();
        void growRemoteKey0(bytes::Alter& data);
        void growRemoteKey(bytes::Alter& data, bool isStatic);

    private:
        using PublicKey = Secret<_publicKeySize>;
        using PrivateKey = Secret<_privateKeySize>;
        struct KeyPair
        {
            PublicKey   _public;
            PrivateKey  _private;
        };

    private:
        void updateInputKey();
        void updateOutputKey();
        void updateKey(const KeyPair& asymLocal, Symmetric* target);

        void updateBothKeys();

    private:
        apip::PrivateKey    _authLocal;
        apip::PublicKey     _authRemote;
        bool                _remoteAuthentificationInitiated = false;
        bool                _remoteAuthentificated = false;

    private:
        std::queue<KeyPair> _asymLocal;
        uint64              _asymLocalAge = 0;

        PublicKey           _asymRemote;
        uint32              _asymRemote0ReceivedSize = 0;
        uint64              _asymRemoteAge = 0;

        bool                _protocolMarkerSent = false;

    private:
        Protocol *                  _protocol = nullptr;
        stages::in::Ciphering *     _inCiphering = nullptr;
        stages::out::Ciphering *    _outCiphering = nullptr;

    private:
        uint64  _inputTrafficCounter = 0;
        uint64  _inputTrafficSize = 0;

        uint64  _outputTrafficCounter = 0;
        uint64  _outputTrafficSize = 0;
    };

    using HandshakePtr = std::unique_ptr<Handshake>;
}
