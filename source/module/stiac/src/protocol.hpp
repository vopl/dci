/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"

#include "remoteEdge.hpp"
#include "localEdge.hpp"

#include "stages/in/ciphering.hpp"
#include "stages/in/cutting.hpp"
#include "stages/in/compression.hpp"

#include "stages/out/ciphering.hpp"
#include "stages/out/cutting.hpp"
#include "stages/out/compression.hpp"

#include "crypto/handshake.hpp"

namespace dci::module::stiac
{
    class Protocol
        : public api::Protocol<>::Opposite
        , public host::module::ServiceBase<Protocol>
    {
    public:
        Protocol();
        ~Protocol();

    public:
        void localEdgeWantRemove(LocalEdge* instance);

        void internalError(stages::Base* instance, const std::string& details);
        void localEdgeFail(LocalEdge* instance, const std::string& details);
        void decompressionFail(stages::Base* instance);
        void integrityViolation(stages::Base* instance);

        std::vector<uint8> protocolMarker();
        void handshakeProtocolMarker(std::vector<uint8> remote);
        void handshakeAuthentificated();
        void handshakeFail(const std::string& details);
        void decipheringFail(const std::string& details);

        void remoteEdgeWantRemove(RemoteEdge* instance);

        void linkHasOutput(stages::Base* link);

    private:
        void paramsChanged(uint32 epc);
        bool buildChain();

        void fail(auto&& err);
        void pause();

    private:
        void push2Chain(auto& linkPtr, auto&&... args);

    private:
        void updateAutoPumping();
        void instantPumpRequested();

        void doPump();

    private:
        struct Marker
        {
            uint8 _version              = 0;
            uint8 _inputRequirements    = 0;
            uint8 _outputRequirements   = 0;
        };

    private://задиктованные пользователем параметры
        api::RemoteEdge<>::Opposite     _paramRemoteEdge;

        apip::Requirements              _paramInputRequirements         = apip::Requirements::null;
        apip::Requirements              _paramInputOptionalRequirements = apip::Requirements::null;
        apip::Requirements              _paramOutputRequirements        = apip::Requirements::null;

        Bytes                           _paramAuthPrologue;
        apip::PrivateKey                _paramAuthLocal {};

        api::LocalEdge<>::Opposite      _paramLocalEdge;

        apip::AutoPumping               _paramAutoPumping = apip::AutoPumping::instantly;

    private:
        apip::Requirements              _effectiveInputRequirements     = apip::Requirements::null;
        apip::Requirements              _effectiveOutputRequirements    = apip::Requirements::null;

        apip::Requirements              _remoteOutputRequirements       = apip::Requirements::null;

    private:

        enum ParamsChanging : uint32
        {
            epc_remoteEdge                  = uint32(1) << 0,
            epc_inputRequirements           = uint32(1) << 1,
            epc_inputOptionalRequirements   = uint32(1) << 2,
            epc_outputRequirements          = uint32(1) << 3,
            epc_authPrologue                = uint32(1) << 4,
            epc_authLocal                   = uint32(1) << 5,
            epc_localEdge                   = uint32(1) << 6,
        };

        uint32 _paramsChanging = ~uint32();

    private:
        apip::State _state = apip::State::null;

    private:
        //процессоры
        RemoteEdgePtr                   _remoteEdge;

        stages::in::CipheringPtr        _inCiphering;
        stages::in::CuttingPtr          _inCutting;
        stages::in::CompressionPtr      _inCompression;

        LocalEdgePtr                    _localEdge;

        stages::out::CompressionPtr     _outCompression;
        stages::out::CuttingPtr         _outCutting;
        stages::out::CipheringPtr       _outCiphering;
        //и закольцовано на remoteEdge


        //актуальная цепь из непустых процессоров и хакеров
        std::vector<stages::Base*>  _chain;
        uint64                      _hasOutputFlags = 0;
        bool                        _pumpingInProgress = false;
        poll::Timer                 _delayedAutoPumpTicker{std::chrono::milliseconds{0}, false, [this]{doPump();}};

        crypto::HandshakePtr        _handshake;

        std::queue<cmt::Promise<apip::PublicKey>> _remoteAuthWaiters;
    };
}
