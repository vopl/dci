// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
