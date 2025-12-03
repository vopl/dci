// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "handshake.hpp"
#include "stages/in/ciphering.hpp"
#include "stages/out/ciphering.hpp"
#include "../protocol.hpp"

namespace dci::module::stiac::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        bool isZeros(const uint8* data, uint32 size)
        {
            volatile const uint8* vdata = const_cast<volatile const uint8 *>(data);

            uint8 sum = 0;

            for(uint32 i=0; i!=size; ++i)
            {
                sum |= vdata[i];
            }

            return 0 == sum;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Handshake::Handshake(
            Protocol* protocol,
            stages::in::Ciphering* inCiphering,
            stages::out::Ciphering* outCiphering)
        : _protocol(protocol)
        , _inCiphering(inCiphering)
        , _outCiphering(outCiphering)
    {
        _inCiphering->setHandshake(this);
        _outCiphering->setHandshake(this);

        _inCiphering->mixHash("dci/module/stiac/0");
        _outCiphering->mixHash("dci/module/stiac/0");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Handshake::~Handshake()
    {
        cleanMemoryUnder(_authLocal);
        cleanMemoryUnder(_authRemote);

        cleanMemoryUnder(_remoteAuthentificationInitiated);
        cleanMemoryUnder(_remoteAuthentificated);

        std::queue<KeyPair>().swap(_asymLocal);
        cleanMemoryUnder(_asymLocalAge);

        //_asymRemote
        cleanMemoryUnder(_asymRemote0ReceivedSize);
        cleanMemoryUnder(_asymRemoteAge);

        cleanMemoryUnder(_protocolMarkerSent);

        cleanMemoryUnder(_protocol);
        cleanMemoryUnder(_inCiphering);
        cleanMemoryUnder(_outCiphering);

        cleanMemoryUnder(_inputTrafficCounter);
        cleanMemoryUnder(_inputTrafficSize);

        cleanMemoryUnder(_outputTrafficCounter);
        cleanMemoryUnder(_outputTrafficSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::setAuth(const Bytes& prologue, const apip::PrivateKey& local)
    {
        _inCiphering->resetState();
        _outCiphering->resetState();

        _inCiphering->mixHash("dci/module/stiac/0");
        _outCiphering->mixHash("dci/module/stiac/0");

        if(!prologue.empty())
        {
            _inCiphering->mixHash(prologue);
            _outCiphering->mixHash(prologue);
        }

        _authLocal = local;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    apip::PublicKey Handshake::remoteAuth()
    {
        if(_remoteAuthentificated)
        {
            return _authRemote;
        }

        return apip::PublicKey();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::start()
    {
        if(!_asymLocalAge)
        {
            growLocalKey();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::someInputMessageDeciphered()
    {
        if(_remoteAuthentificationInitiated)
        {
            //есть успешный трафик после применения аутентификационного ключа удаленной стороны
            //принять аутентификацию
            _remoteAuthentificationInitiated = false;
            _remoteAuthentificated = true;
            _protocol->handshakeAuthentificated();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::outputHasDisallowedPayload()
    {
        if(!_asymLocalAge)
        {
            growLocalKey();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::inputComing(MessageType messageType, bytes::Alter& data)
    {
        switch(messageType)
        {
        case MessageType::fakeNull:
            if(!_asymLocalAge)
            {
                growLocalKey();
            }

            growRemoteKey0(data);
            break;

        case MessageType::protocolMarker:
            {
                std::vector<uint8> pm(data.size());
                data.removeTo(pm.data());
                _protocol->handshakeProtocolMarker(std::move(pm));
            }
            break;

        case MessageType::ekey:
        case MessageType::skey:
            dbgAssert(_publicKeySize == data.size());
            growRemoteKey(data, MessageType::skey==messageType);
            break;

        case MessageType::keyApplied:
            dbgAssert(0 == data.size());
            cropLocalKey();
            break;

        default:
            _protocol->handshakeFail("bad data from remote");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::outputTraffic(uint32 size)
    {
        _outputTrafficCounter++;
        _outputTrafficSize += size;

        if(1024         <= _outputTrafficCounter ||
           1024*1024*8  <= _outputTrafficSize)
        {
            growLocalKey();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::inputTraffic(uint32 size)
    {
        _inputTrafficCounter++;
        _inputTrafficSize += size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::growLocalKey()
    {
        _asymLocalAge++;
        _asymLocal.emplace();
        KeyPair& kp = _asymLocal.back();

        MessageType mt = MessageType::ekey;

        //статический или эфемерный
        if(2 == _asymLocalAge && !_authLocal.empty())
        {
            //второй ключ - статический
            kp._private = _authLocal;

            dci::crypto::curve25519::basepoint(kp._private.data(), kp._public.data());
            if(crypto::isZeros(kp._public.data(), kp._public.size()))
            {
                _protocol->handshakeFail("bad local authentification key");
                return;
            }

            mt = MessageType::skey;
        }
        else
        {
            //эфемерный
            do
            {
                bool b = dci::crypto::rnd::generate(kp._private.data(), kp._private.size());
                if(!b)
                {
                    _protocol->handshakeFail("rnd failed");
                    return;
                }

                dci::crypto::curve25519::basepoint(kp._private.data(), kp._public.data());
            }
            while(isZeros(kp._public.data(), kp._public.size()));

            if(1 == _asymLocalAge)
            {
                //первый ключ передается как сырые данные
                mt = MessageType::fakeNull;
            }
            else
            {
                mt = MessageType::ekey;
            }
        }

        _outCiphering->urgent(mt, kp._public.data(), kp._public.size());

        if(_asymRemoteAge)
        {
            updateOutputKey();

            if(!_protocolMarkerSent)
            {
                const auto& pm = _protocol->protocolMarker();
                _outCiphering->urgent(MessageType::protocolMarker, pm.data(), static_cast<uint32>(pm.size()));
                _protocolMarkerSent = true;
            }

            if(_authLocal.empty() || MessageType::skey == mt)
            {
                _outCiphering->allowPayload();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::cropLocalKey()
    {
        dbgAssert(_asymLocal.size()>1);
        if(_asymLocal.size() <= 1)
        {
            //dbgWarn("рассинхронизация по отосланым ключам и подтверждениям о их получении, в штатной ситуации такого не может быть");
            _protocol->handshakeFail("remote desynced");
            return;
        }

        _asymLocal.pop();

        updateInputKey();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::growRemoteKey0(bytes::Alter& data)
    {
        uint32 amount = std::min(_publicKeySize-_asymRemote0ReceivedSize, data.size());
        dbgAssert(amount);

        data.removeTo(&_asymRemote[_asymRemote0ReceivedSize], amount);
        _asymRemote0ReceivedSize += amount;

        if(_asymRemote0ReceivedSize < _publicKeySize)
        {
            return;
        }

        _asymRemoteAge++;
        dbgAssert(_asymLocalAge);

        if(1 == _asymLocal.size())
        {
            updateBothKeys();
        }
        else
        {
            updateInputKey();
            updateOutputKey();
        }

        if(!_authLocal.empty())
        {
            growLocalKey();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::growRemoteKey(bytes::Alter& data, bool isStatic)
    {
        dbgAssert(_asymRemoteAge);
        dbgAssert(_publicKeySize == data.size());

        data.removeTo(_asymRemote.data(), _asymRemote.size());

        _asymRemoteAge++;
        dbgAssert(_asymLocalAge);

        _outCiphering->urgent(MessageType::keyApplied, nullptr, 0);

        if(1 == _asymLocal.size())
        {
            updateBothKeys();
        }
        else
        {
            updateInputKey();
            updateOutputKey();
        }

        if(isStatic)
        {
            _remoteAuthentificationInitiated = true;
            _authRemote = _asymRemote;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::updateInputKey()
    {
        dbgAssert(_asymLocalAge);
        dbgAssert(!_asymLocal.empty());

        updateKey(_asymLocal.front(), _inCiphering);

        _inputTrafficCounter = 0;
        _inputTrafficSize = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::updateOutputKey()
    {
        dbgAssert(_asymLocalAge);
        dbgAssert(!_asymLocal.empty());

        updateKey(_asymLocal.back(), _outCiphering);

        _outputTrafficCounter = 0;
        _outputTrafficSize = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::updateKey(const KeyPair& asymLocal, crypto::Symmetric* target)
    {
        dbgAssert(_asymRemoteAge);

        if(asymLocal._public < _asymRemote)
        {
            target->mixHash(asymLocal._public.data(), asymLocal._public.size());
            target->mixHash(_asymRemote.data(), _asymRemote.size());
        }
        else
        {
            target->mixHash(_asymRemote.data(), _asymRemote.size());
            target->mixHash(asymLocal._public.data(), asymLocal._public.size());
        }

        uint8 sym[_symmetricKeySize];
        dci::crypto::curve25519::donna(_asymRemote.data(), asymLocal._private.data(), sym);

        if(isZeros(sym, sizeof(sym)))
        {
            _protocol->handshakeFail("bad remote public key detected");
            return;
        }

        target->mixKey(sym);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handshake::updateBothKeys()
    {
        dbgAssert(_asymRemoteAge);
        dbgAssert(_asymLocalAge);
        dbgAssert(1 == _asymLocal.size());

        {
            const KeyPair& asymLocal = _asymLocal.back();

            if(asymLocal._public < _asymRemote)
            {
                _outCiphering->mixHash(asymLocal._public.data(), asymLocal._public.size());
                _outCiphering->mixHash(_asymRemote.data(), _asymRemote.size());
                _inCiphering->mixHash(asymLocal._public.data(), asymLocal._public.size());
                _inCiphering->mixHash(_asymRemote.data(), _asymRemote.size());
            }
            else
            {
                _outCiphering->mixHash(_asymRemote.data(), _asymRemote.size());
                _outCiphering->mixHash(asymLocal._public.data(), asymLocal._public.size());
                _inCiphering->mixHash(_asymRemote.data(), _asymRemote.size());
                _inCiphering->mixHash(asymLocal._public.data(), asymLocal._public.size());
            }

            uint8 sym[_symmetricKeySize];
            dci::crypto::curve25519::donna(_asymRemote.data(), asymLocal._private.data(), sym);

            if(isZeros(sym, sizeof(sym)))
            {
                _protocol->handshakeFail("bad remote public key detected");
                return;
            }

            _outCiphering->mixKey(sym);
            _inCiphering->mixKey(sym);
        }

        _outputTrafficCounter = 0;
        _outputTrafficSize = 0;

        _inputTrafficCounter = 0;
        _inputTrafficSize = 0;
    }

}
