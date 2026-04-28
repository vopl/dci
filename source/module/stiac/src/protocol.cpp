/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "protocol.hpp"
#include "crypto/secret.hpp"

namespace dci::module::stiac
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Protocol::Protocol()
        : api::Protocol<>::Opposite(interface::Initializer{})
    {

        ////текущая версия протокола
        //in getVersion() -> uint32;
        methods()->getVersion() += serviceSol() * []()
        {
            return readyFuture(uint32(0));
        };

        ////установить конечные точки
        //in setRemoteEdge(RemoteEdge::Opposite remote);
        methods()->setRemoteEdge() += serviceSol() * [this](api::RemoteEdge<>::Opposite remote)
        {
            if(_paramRemoteEdge != remote)
            {
                _paramRemoteEdge = std::move(remote);
                paramsChanged(epc_remoteEdge);
            }
        };

        ////установить требования (наличие обфускатора например...)
        //in setRequirements(protocol::Requirements input, protocol::Requirements output);
        methods()->setRequirements() += serviceSol() * [this](
                                  apip::Requirements input,
                                  apip::Requirements inputOptional,
                                  apip::Requirements output)
        {
            if(_paramInputRequirements != input)
            {
                _paramInputRequirements = input;
                paramsChanged(epc_inputRequirements);
            }

            if(_paramInputOptionalRequirements != inputOptional)
            {
                _paramInputOptionalRequirements = inputOptional;
                paramsChanged(epc_inputOptionalRequirements);
            }

            if(_paramOutputRequirements != output)
            {
                _paramOutputRequirements = output;
                paramsChanged(epc_outputRequirements);
            }
        };

        ////установить заранее известные ключи если есть
        //in setAuthPrologue(bytes);
        methods()->setAuthPrologue() += serviceSol() * [this](Bytes prologue)
        {
            if(_paramAuthPrologue != prologue)
            {
                _paramAuthPrologue = std::move(prologue);
                paramsChanged(epc_authPrologue);
            }
        };

        //in setAuthLocal(protocol::PrivateKey local);
        methods()->setAuthLocal() += serviceSol() * [this](apip::PrivateKey local)
        {
            if(_paramAuthLocal != local)
            {
                _paramAuthLocal = std::move(local);
                paramsChanged(epc_authLocal);
            }
        };

        //in setLocalEdge(LocalEdge::Opposite local);
        methods()->setLocalEdge() += serviceSol() * [this](api::LocalEdge<>::Opposite local)
        {
            if(_paramLocalEdge != local)
            {
                _paramLocalEdge = std::move(local);
                paramsChanged(epc_localEdge);
            }
        };

        //in setAutoPumping(protocol::AutoPumping);
        methods()->setAutoPumping() += serviceSol() * [this](apip::AutoPumping autoPumping)
        {
            if(_paramAutoPumping != autoPumping)
            {
                _paramAutoPumping = autoPumping;
                updateAutoPumping();
            }
        };

        //in start();
        methods()->start() += serviceSol() * [this]()
        {
            switch(_state)
            {
            case apip::State::work:
                return;

            case apip::State::null:
            case apip::State::pause:
            case apip::State::fail:
                break;
            }

            if(!buildChain())
            {
                return;
            }

            if(_localEdge)
            {
                _localEdge->start();
            }

            if(apip::State::work != _state)
            {
                _state = apip::State::work;

    //            if(_handshake)
    //            {
    //                _handshake->start();
    //            }

                updateAutoPumping();
                methods()->stateChanged(_state);
            }
        };

        //in pump();
        methods()->pump() += serviceSol() * [this]()
        {
            doPump();
        };

        //in state() -> protocol::State;
        methods()->state() += serviceSol() * [this]()
        {
            return readyFuture(_state);
        };

        //out statusChanged(protocol::State state);

        //in remoteAutorization() -> protocol::Key;
        methods()->remoteAuth() += serviceSol() * [this]()
        {
            if(_handshake)
            {
                return readyFuture(_handshake->remoteAuth());
            }

            _remoteAuthWaiters.emplace();
            return _remoteAuthWaiters.back().future();
        };

        //out remoteAutorizationChanged(protocol::Key remotePublic);

        //out failed(exception);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Protocol::~Protocol()
    {
        pause();
        serviceSol().flush();

        dbgAssert(!_pumpingInProgress);
        _hasOutputFlags = 0;
        _autoPumpActivator.reset();
        _autoPumpOwner.stop();

        while(!_remoteAuthWaiters.empty())
        {
            _remoteAuthWaiters.pop();
        }

        _chain.clear();
        _handshake.reset();

        _localEdge.reset();
        _remoteEdge.reset();

        _inCiphering.reset();
        _inCutting.reset();
        _inCompression.reset();

        _outCompression.reset();
        _outCutting.reset();
        _outCiphering.reset();

        crypto::cleanMemoryUnder(_paramAuthPrologue);
        crypto::cleanMemoryUnder(_paramAuthLocal);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::localEdgeWantRemove(LocalEdge* instance)
    {
        (void)instance;
        dbgAssert(_localEdge.get() == instance);

        _localEdge.reset();
        pause();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::internalError(stages::Base* instance, const std::string& details)
    {
        (void)instance;
        fail(apip::InternalError(details));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::localEdgeFail(LocalEdge* instance, const std::string& details)
    {
        (void)instance;
        dbgAssert(_localEdge.get() == instance);
        fail(apip::LocalEdgeFail(details));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::decompressionFail(stages::Base* instance)
    {
        (void)instance;
        fail(apip::DecompressionFail());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::integrityViolation(stages::Base* instance)
    {
        (void)instance;
        fail(apip::IntegrityViolation());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::vector<uint8> Protocol::protocolMarker()
    {
        Marker m
        {
            ._version = 0,
            ._inputRequirements = static_cast<uint8>(_paramInputRequirements),
            ._outputRequirements = static_cast<uint8>(_paramOutputRequirements)
        };

        std::vector<uint8> res(sizeof(Marker));
        std::memcpy(res.data(), &m, sizeof(Marker));
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::handshakeProtocolMarker(std::vector<uint8> remote)
    {
        static_assert(3 == sizeof(Marker));
        if(3 != remote.size())
        {
            if(1 <= remote.size())
            {
                apip::BadRemoteVersion e;
                e.version = remote[0];
                fail(e);
                return;
            }

            apip::BadRemoteMarker e;
            fail(e);
            return;
        }

        Marker m;
        std::memcpy(&m, remote.data(), sizeof(Marker));

        if(m._version != 0)
        {
            apip::BadRemoteVersion e;
            e.version = m._version;
            fail(e);
            return;
        }

        apip::Requirements rOut = static_cast<apip::Requirements>(m._outputRequirements);
        apip::Requirements rInp = static_cast<apip::Requirements>(m._inputRequirements);

        if((rOut & ~(_paramInputRequirements | _paramInputOptionalRequirements)) ||
           (rInp & ~_paramOutputRequirements))
        {
            apip::BadRemoteRequirements e;
            e.input = rInp;
            e.output = rOut;
            fail(e);
            return;
        }

        if(_remoteOutputRequirements != rOut)
        {
            _remoteOutputRequirements = rOut;

            if(_effectiveInputRequirements != rOut)
            {
                _autoPumpActivator.reset();
                _chain.clear();

                if(!buildChain())
                {
                    return;
                }

                updateAutoPumping();
            }
        }

        dbgAssert(_effectiveInputRequirements == rOut);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::handshakeAuthentificated()
    {
        dbgAssert(_handshake);
        apip::PublicKey remote = _handshake->remoteAuth();
        dbgAssert(crypto::Handshake::_publicKeySize == remote.size());

        while(!_remoteAuthWaiters.empty())
        {
            _remoteAuthWaiters.front().resolveValue(remote);
            _remoteAuthWaiters.pop();
        }

        methods()->remoteAuthChanged(remote);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::handshakeFail(const std::string& details)
    {
        fail(apip::HandshakeFail(details));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::decipheringFail(const std::string& details)
    {
        fail(apip::DecipheringFail(details));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::remoteEdgeWantRemove(RemoteEdge* instance)
    {
        (void)instance;
        dbgAssert(_remoteEdge.get() == instance);

        _remoteEdge.reset();
        pause();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::linkHasOutput(stages::Base* link)
    {
        dbgAssert(_chain.size() > link->getIndexInChain());
        dbgAssert(_chain[link->getIndexInChain()] == link);

        _hasOutputFlags |= (1ull << link->getIndexInChain());

        instantPumpRequested();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::paramsChanged(uint32 epc)
    {
        if(epc)
        {
            _paramsChanging |= epc;
            _autoPumpActivator.reset();
            _chain.clear();
            _handshake.reset();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Protocol::buildChain()
    {
        dbgAssert(_chain.empty());
        _autoPumpActivator.reset();
        _chain.clear();
        _hasOutputFlags = 0;

        {// ограничения

            _effectiveOutputRequirements = _paramOutputRequirements;
            _effectiveInputRequirements = _paramInputRequirements | (_paramInputOptionalRequirements & _remoteOutputRequirements);

            //шифрование должно быть одинаковым с обоих сторон
            if(
               (apip::Requirements::ciphering & _effectiveInputRequirements) !=
               (apip::Requirements::ciphering & _effectiveOutputRequirements))
            {
                fail(apip::BadRequirements("ciphering requirement must be equal for input and output"));
                return false;
            }

            //... еще?
        }

        ////////////////////////////////////////////////////
        // remote edge
        {
            if(!_paramRemoteEdge)
            {
                fail(apip::MissingRemoteEdge());
                return false;
            }

            if(epc_remoteEdge & _paramsChanging)
            {
                _remoteEdge.reset();
            }

            push2Chain(_remoteEdge, this, _paramRemoteEdge);
        }

        ////////////////////////////////////////////////////
        // inCiphering
        if(apip::Requirements::ciphering == (apip::Requirements::ciphering & _effectiveInputRequirements))
        {
            push2Chain(_inCiphering, this);
        }
        else
        {
            ////////////////////////////////////////////////////
            // inCutting
            if(apip::Requirements::cutting == (apip::Requirements::cutting & _effectiveInputRequirements))
            {
                push2Chain(_inCutting,
                           this,
                           apip::Requirements::integrity == (apip::Requirements::integrity & _effectiveInputRequirements));
            }
        }

        ////////////////////////////////////////////////////
        // inCompression
        if(!!(apip::Requirements::compression & _effectiveInputRequirements))
        {
            push2Chain(_inCompression, this);
        }

        ////////////////////////////////////////////////////
        // localEdge
        {
            if(!_paramLocalEdge)
            {
                fail(apip::MissingLocalEdge());
                return false;
            }

            if(epc_localEdge & _paramsChanging)
            {
                _localEdge.reset();
            }

            push2Chain(_localEdge, this, _paramLocalEdge);
        }

        ////////////////////////////////////////////////////
        // outCompression
        if(!!(apip::Requirements::compression & _effectiveOutputRequirements))
        {
            push2Chain(_outCompression, this);
        }

        if(apip::Requirements::ciphering == (apip::Requirements::ciphering & _effectiveOutputRequirements))
        {
            ////////////////////////////////////////////////////
            // outCiphering
            push2Chain(_outCiphering, this);
        }
        else
        {
            ////////////////////////////////////////////////////
            // outCutting
            if(apip::Requirements::cutting == (apip::Requirements::cutting & _effectiveOutputRequirements))
            {
                push2Chain(_outCutting,
                           this,
                           apip::Requirements::integrity == (apip::Requirements::integrity & _effectiveOutputRequirements));
            }
        }

        ////////////////////////////////////////////////////
        //initialize stages
        for(std::size_t i(_chain.size()-1); i<_chain.size(); --i)
        {
            if(!_chain[i]->initialize())
            {
                internalError(_chain[i], "initialization failed");
                return false;
            }
        }

        ////////////////////////////////////////////////////
        //propagate empty prefix size
        for(std::size_t i(_chain.size()-1); i<_chain.size(); --i)
        {
            uint16 size;
            if(i < _chain.size()-1)
            {
                size = _chain[i+1]->getWantedEmptyPrefix();
            }
            else
            {
                size = _chain[0]->getWantedEmptyPrefix();
            }

            _chain[i]->setWantedEmptyPrefix(size);
        }

        ////////////////////////////////////////////////////
        if(apip::Requirements::ciphering == (apip::Requirements::ciphering & _effectiveInputRequirements))
        {
            if(!_handshake)
            {
                _handshake.reset(new crypto::Handshake(
                                     this,
                                     _inCiphering.get(),
                                     _outCiphering.get()));

                _handshake->setAuth(
                            _paramAuthPrologue,
                            _paramAuthLocal);
            }
        }

        ////////////////////////////////////////////////////
        _paramsChanging = 0;

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::fail(auto&& err)
    {
        api::Protocol<>::Opposite face = *this;

        if(apip::State::fail != _state)
        {
            _state = apip::State::fail;
            updateAutoPumping();

            if(_localEdge)
            {
                _localEdge->pause();
            }

            face->stateChanged(_state);
        }

        face->failed(std::make_exception_ptr(std::move(err)));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::pause()
    {
        switch(_state)
        {
        case apip::State::null:
            return;

        case apip::State::work:
            _state = apip::State::pause;
            updateAutoPumping();

            if(_localEdge)
            {
                _localEdge->pause();
            }

            methods()->stateChanged(_state);
            return;

        case apip::State::pause:
        case apip::State::fail:
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::push2Chain(auto& linkPtr, auto&&... args)
    {
        if(!linkPtr)
        {
            linkPtr.reset(new typename std::remove_reference_t<decltype(linkPtr)>::element_type(std::forward<decltype(args)>(args)...));
        }

        linkPtr->setIndexInChain(_chain.size());

        _chain.push_back(linkPtr.get());

        if(linkPtr->hasOutput())
        {
            linkHasOutput(linkPtr.get());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::updateAutoPumping()
    {
        switch(_paramAutoPumping)
        {
        case apip::AutoPumping::none:
            _autoPumpActivator.reset();
            _autoPumpOwner.stop();
            break;

        case apip::AutoPumping::instantly:
            _autoPumpActivator.reset();
            _autoPumpOwner.stop();
            if(apip::State::work == _state)
            {
                doPump();
            }
            break;

        case apip::AutoPumping::delayed:
            if(apip::State::work == _state)
            {
                _autoPumpActivator.raise();
            }
            else
            {
                _autoPumpActivator.reset();
            }

            if(_autoPumpOwner.empty())
            {
                cmt::spawn() += _autoPumpOwner * [this]
                {
                    for(;;)
                    {
                        _autoPumpActivator.wait();
                        doPump();
                    }
                };
            }
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::instantPumpRequested()
    {
        if(apip::State::work != _state)
        {
            return;
        }

        switch(_paramAutoPumping)
        {
        case apip::AutoPumping::instantly:
            doPump();
            break;

        case apip::AutoPumping::delayed:
            _autoPumpActivator.raise();
            break;

        default:
            //ignore
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Protocol::doPump()
    {
        if(_pumpingInProgress)
        {
            return;
        }

        auto lifeLocker = opposite();

        _pumpingInProgress = true;
        utils::AtScopeExit cleaner{[&]
        {
            _pumpingInProgress = false;
        }};

        while(apip::State::work == _state && _hasOutputFlags)
        {

            std::size_t index = utils::bits::count0Least(_hasOutputFlags);
            _hasOutputFlags ^= (1ull << index);

            dbgAssert(index < _chain.size());

            if(index == _chain.size()-1)
            {
                _chain[0]->input(_chain[index]->flushOutput());
            }
            else
            {
                _chain[index+1]->input(_chain[index]->flushOutput());
            }
        }

        return;
    }
}
