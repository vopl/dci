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
#include "stages/base.hpp"

#include "localEdge/input.hpp"
#include "localEdge/output.hpp"
#include "localEdge/localLinks.hpp"
#include "localEdge/remoteLinks.hpp"
#include "localEdge/duty.hpp"

namespace dci::module::stiac
{
    class LocalEdge
        : public stages::Base
        , private sbs::Owner
        , public localEdge::Input
        , public localEdge::Output
        , public link::Hub4Link
    {
        using Input     = localEdge::Input;
        using Output    = localEdge::Output;

    public:
        LocalEdge(Protocol* protocol, const api::LocalEdge<>::Opposite& interface);
        ~LocalEdge() override;

        void start();
        void pause();

    private:// Base
        uint16 getWantedEmptyPrefix() const override;
        void input(Bytes&& msg) override;
        Bytes flushOutput() override;

    private:// Hub4Link
        link::Sink makeSink(link::Id id) override;
        void linkUninvolved(link::Id id, int uf) override;

    private:// Hub4Source
        bool emplaceLink(link::BasePtr&& link, link::RemoteId remoteId) override;
        void finalize(link::Source& source, bytes::Alter&& buffer) override;

    private:// Hub4Sink
        link::LocalId emplaceLink(link::BasePtr&& link) override;
        void finalize(link::Sink& sink, bytes::Alter&& buffer) override;

    public:// for Duty
        cmt::Future<None> oppositePutInterface(Interface&& interface);
        void oppositeOptimisticPutInterface(Interface&& interface);

        void oppositeLinkBeginRemove(link::LocalId localId);
        void oppositeLinkBeginRemove(link::RemoteId remoteId);

        void oppositeLinkEndRemove(link::LocalId localId);
        void oppositeLinkEndRemove(link::RemoteId remoteId);

    private:
        api::LocalEdge<>::Opposite  _interface;

        apil::State                 _state = apil::State::null;

        localEdge::LocalLinks       _localLinks;
        localEdge::RemoteLinks      _remoteLinks;

        bool                        _inputProcessingActive = false;

    private:
        localEdge::Duty _duty{this};
    };

    using LocalEdgePtr = std::unique_ptr<LocalEdge>;
}
