// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "peer/io.hpp"

namespace dci::module::ppn::discovery
{
    class Peer
        : public idl::gen::ppn::discovery::Peer<>::Opposite
        , public host::module::ServiceBase<Peer>
    {
    public:
        Peer();
        ~Peer();

    private:
        void joined(const node::link::Id& id, const node::link::Remote<>& r);

    private:
        friend class peer::Io;
        void free(const node::link::Remote<>& r);
        void input(const transport::Address& remoteAddr, const node::link::Id& remoteId, transport::Address&& remoteAddr2);
        List<transport::Address> output(const transport::Address& remoteAddr);

    private:
        void regularGet();
        void setIntensity(double v);

    private:
        bool                                _started {};
        Set<transport::Address>             _localAddresses;

    private:
        node::feature::RemoteAddressSpace<> _ras;

        using Ios = Map<node::link::Remote<>, peer::Io>;
        Ios                                 _ios;

        Ios::iterator                       _nextIos4RegularGet;
        poll::Timer                         _regularGetTicker{std::chrono::seconds{1}, true, [this]{regularGet();}};
    };
}
