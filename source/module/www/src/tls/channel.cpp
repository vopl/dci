// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channel.hpp"
#include "utils.hpp"

namespace dci::module::www::tls
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        int s_BIO_writeEx(BIO* bio, const char* data, std::size_t dlen, std::size_t* written)
        {
            Channel* channel = static_cast<Channel*>(BIO_get_data(bio));
            dbgAssert(dlen <= std::numeric_limits<uint32>::max());
            auto [channelOk, channelWritten] = channel->bioAskWrite(data, dlen);
            if(written)
                *written = channelWritten;
            if(channelOk && !channelWritten)
                BIO_set_retry_write(bio);
            return channelOk ? 1 : -1;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        int s_BIO_readEx(BIO* bio, char* data, std::size_t dlen, std::size_t* readed)
        {
            Channel* channel = static_cast<Channel*>(BIO_get_data(bio));
            dbgAssert(dlen <= std::numeric_limits<uint32>::max());
            auto [channelOk, channelReaded] = channel->bioAskRead(data, dlen);
            if(readed)
                *readed = channelReaded;
            if(channelOk && !channelReaded)
                BIO_set_retry_read(bio);
            return channelOk ? 1 : -1;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        long s_BIO_ctrl(BIO* bio, int cmd, long /*larg*/, void* /*parg*/)
        {
            switch(cmd)
            {
            default:
            case BIO_CTRL_PUSH:
            case BIO_CTRL_POP:
                return 0;
            case BIO_CTRL_FLUSH:
                return 1;
            case BIO_CTRL_EOF:
                {
                    Channel* channel = static_cast<Channel*>(BIO_get_data(bio));
                    return channel->bioAskEof();
                }
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        BIO_METHOD* biom()
        {
            static std::unique_ptr<BIO_METHOD, void(*)(BIO_METHOD*)> s_raii = []
            {
                BIO_METHOD* biom = BIO_meth_new(0|BIO_TYPE_SOURCE_SINK, "dci-module-www-tls-bio");

                BIO_meth_set_write_ex(biom, &s_BIO_writeEx);
                BIO_meth_set_read_ex(biom, &s_BIO_readEx);
                BIO_meth_set_ctrl(biom, &s_BIO_ctrl);

                return std::unique_ptr<BIO_METHOD, void(*)(BIO_METHOD*)>{biom, &BIO_meth_free};
            }();

            return s_raii.get();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(SSL_CTX* sslCtx, const Settings& settings, const api::tls::Channel<>::Opposite& user, api::stream::Channel<>&& peer)
        : _settings{settings}
        , _user{user}
        , _peer{std::move(peer)}
        , _peerBio{BIO_new(biom()), [](BIO* bio){ BIO_free(bio);}}
        , _ssl{SSL_new(sslCtx), &SSL_free}
    {
        BIO_set_data(_peerBio.get(), this);
        BIO_up_ref(_peerBio.get());// SSL_set_bio owns _peerBio pointer without reference increment
        SSL_set_bio(_ssl.get(), _peerBio.get(), _peerBio.get());


        {
            _peer->received() += _sol * [this](Bytes&& data)
            {
                _peerReceived.end().write(std::move(data));
                fsmTick();
            };
            _peer->failed() += _sol * [this](ExceptionPtr&& fail)
            {
                if(!_peerFail && fail)
                    _peerFail = std::move(fail);
                BIO_clear_retry_flags(_peerBio.get());
                cleanup(false, exception::buildInstance<api::tls::error::DownstreamFailed>(_peerFail));
            };
            _peer->closed() += _sol * [this]()
            {
                _peerClosed = true;
                BIO_clear_retry_flags(_peerBio.get());

                switch(_sslState)
                {
                case SslState::shutdown:
                case SslState::done:
                    cleanup(false, {});
                    break;
                default:
                    cleanup(false, exception::buildInstance<api::tls::error::DownstreamClosed>(_peerFail));
                }
            };
            _peer->startReceive();
        }

        {
            _user->alpnProtoSelected() += _sol * [this]()
            {
                return cmt::readyFuture(_alpnProtoSelected);
            };

            _user->send() += _sol * [this](Bytes&& data)
            {
                bool wasEmpty = _userWrote.empty();
                _userWrote.end().write(std::move(data));
                if(wasEmpty && SslState::work == _sslState)
                    fsmTick();
            };

            _user->startReceive() += _sol * [this]()
            {
                if(!_userReceiveStarted)
                {
                    _userReceiveStarted = true;
                    if(SslState::work == _sslState)
                        fsmTick();
                }
            };

            _user->stopReceive() += _sol * [this]()
            {
                _userReceiveStarted = false;
            };

            _user->shutdown() += _sol * [this]()
            {
                switch(_sslState)
                {
                case SslState::handshake:
                case SslState::work:
                    _sslState = SslState::shutdown;
                    fsmTick();
                    break;
                default:
                    break;
                }
            };

            _user->close() += _sol * [this]()
            {
                switch(_sslState)
                {
                case SslState::handshake:
                case SslState::work:
                case SslState::shutdown:
                    _sol.flush();
                    _sslState = SslState::done;
                    _userClosed = true;
                    cleanup(false, {});
                    break;
                default:
                    break;
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        _sol.flush();
        if(_peer)
            _peer->stopReceive();
        _ssl.reset();
        _peerBio.reset();
        _solExternal.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner& Channel::sol()
    {
        return _solExternal;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Channel::handshake()
    {
        fsmTick();
        return _handshakePromise.future();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::tuple<bool, uint32> Channel::bioAskWrite(const void* data, uint32 size)
    {
        if(_peerClosed || _peerFail)
            return {false, 0};

        if(size)
            _peer->send(Bytes{data, size});

        return {true, size};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::tuple<bool, uint32> Channel::bioAskRead(void* data, uint32 maxSize)
    {
        if(_peerClosed || _peerFail)
            return {false, 0};

        return {true, _peerReceived.begin().removeTo(data, maxSize)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Channel::bioAskEof()
    {
        return _peerClosed || _peerFail;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::fsmTick()
    {
        for(;;)
        {
            ERR_clear_error();

            int operationResult = 0;
            switch(_sslState)
            {
            case SslState::handshake:
                operationResult = SSL_do_handshake(_ssl.get());
                break;
            case SslState::work:
                {
                    if(_userReceiveStarted && (!_peerReceived.empty() || SSL_has_pending(_ssl.get())))
                    {
                        Bytes received4User;
                        for(;;)
                        {
                            bytes::Alter alter = received4User.end();
                            uint32 bufferSize;
                            void* buffer = alter.prepareWriteBuffer(bufferSize);
                            operationResult = SSL_read(_ssl.get(), buffer, bufferSize);
                            if(operationResult <= 0)
                                break;
                            alter.commitWriteBuffer(operationResult);
                        }

                        if(!received4User.empty())
                            _user->received(std::move(received4User));
                    }
                    else if(!_userWrote.empty())
                    {
                        do
                        {
                            bytes::Alter alter = _userWrote.begin();
                            uint32 bufferSize = alter.continuousDataSize();
                            const void* buffer = alter.continuousData();
                            operationResult = SSL_write(_ssl.get(), buffer, bufferSize);
                            if(operationResult <= 0)
                                break;
                            alter.remove(operationResult);
                        }
                        while(!_userWrote.empty());
                    }
                    else
                        return; // no work to do
                }
                break;
            case SslState::shutdown:
                operationResult = SSL_shutdown(_ssl.get());
                if(!operationResult)
                    operationResult = SSL_shutdown(_ssl.get());
                break;
            case SslState::done:
                return;
            }

            int sslError = SSL_get_error(_ssl.get(), operationResult);
            unsigned long error = ERR_get_error();

            if(operationResult > 0)
            {
                switch(_sslState)
                {
                case SslState::handshake:
                    _sslState = SslState::work;

                    {
                        const unsigned char* proto{};
                        unsigned len{};
                        SSL_get0_alpn_selected(_ssl.get(), &proto, &len);
                        if(proto && len)
                            _alpnProtoSelected.assign(proto, proto+len);
                        else
                            _alpnProtoSelected.clear();
                    }

                    dbgAssert(!_handshakePromise.resolved());
                    if(!_handshakePromise.resolved())
                        _handshakePromise.resolveValue();
                    break;

                case SslState::work:
                    break;

                case SslState::shutdown:
                    _sslState = SslState::done;
                    cleanup(true, {});
                    break;

                case SslState::done:
                    return;
                }
            }

            switch(sslError)
            {
            case SSL_ERROR_NONE:
                return;

            case SSL_ERROR_WANT_READ:
                dbgAssert(!_peerFail);
                //dbgAssert(!_peerClosed);
                dbgAssert(_peerReceived.empty());
                return;// wait data from peer

            case SSL_ERROR_WANT_WRITE:
                unreacheable();
                return;

            case SSL_ERROR_SSL:
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_ZERO_RETURN:
            default:
                {
                    ExceptionPtr fail;
                    if(_peerFail)
                        fail = exception::buildInstance<api::tls::error::DownstreamFailed>(_peerFail);

                    if(!fail && _peerClosed)
                        fail = exception::buildInstance<api::tls::error::DownstreamClosed>();

                    if(error)
                    {
                        if(SslState::handshake == _sslState && ERR_GET_LIB(error) == ERR_LIB_SSL && ERR_GET_REASON(error) == SSL_R_CERTIFICATE_VERIFY_FAILED)
                        {
                            long verifyResult = SSL_get_verify_result(_ssl.get());
                            fail = exception::buildInstance<api::tls::error::PeerVerifyFailed>(fail, std::string_view{X509_verify_cert_error_string(verifyResult)});
                        }
                        else
                            fail = exception::buildInstance<api::tls::Error>(fail, utils::errorString(error));
                    }

                    cleanup(false, fail);
                }
                return;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::cleanup(bool peerShutdown, ExceptionPtr userFail)
    {
        cmt::Promise<> handshakePromise{std::move(_handshakePromise)};

        _sol.flush();

        _userWrote.clear();
        _peerReceived.clear();


        bool peerShuttedDown = _peerShuttedDown;
        _peerShuttedDown = true;
        bool peerClosed = _peerClosed;
        _peerClosed = true;
        api::stream::Channel<> peer = std::move(_peer);

        bool userClosed = _userClosed;
        _userClosed = true;
        api::tls::Channel<>::Opposite user = std::move(_user);


        if(peer)
        {
            peer->stopReceive();

            if(peerShutdown && !peerShuttedDown)
                peer->shutdown();

            if(!peerClosed)
                peer->close();
        }

        if(user)
        {
            if(userFail)
                user->failed(std::move(userFail));

            if(!userClosed)
                user->closed();
        }

        if(!handshakePromise.resolved())
            handshakePromise.resolveException(exception::buildInstance<api::tls::error::HandshakeFailed>(userFail));
    }
}
