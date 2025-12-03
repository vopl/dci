// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "node/netEnumerator.hpp"
#include "node/transportHub.hpp"

namespace dci::module::ppn
{
    class Node
        : public idl::gen::ppn::Node<>::Opposite
        , public host::module::ServiceBase<Node>
    {
    public:
        Node();
        ~Node();

        void start(idl::gen::Config&& config);
        void stop();

    public:
        void localAddressDeclare(const transport::Address& a);
        void localAddressUndeclare(const transport::Address& a);

    private:
        void emitFail(const std::string& comment);
        void emitFail(ExceptionPtr e, const std::string& comment);

    private:
        node::NetEnumerator& netEnumerator();

        transport::Address fixAcceptorAddress(const transport::Address& a);
        transport::Address fixConnectorAddress(const transport::Address& a);

        transport::acceptor::Downstream<> makeAcceptor(const transport::Address& a);
        transport::connector::Downstream<> makeConnector(const transport::Address& a);

    private:
        void csessionWorker(api::link::Id id, const transport::Address& a);
        void asessionWorker(transport::Channel<>&& ch);

        void flushJoinWaiters(const transport::Address& a, ExceptionPtr e);
        void flushJoinWaiters(const transport::Address& a, api::link::Remote<> r);

    private:
        sbs::Owner       _sol;
        cmt::task::Owner _tow;

        bool _started = false;

        List<idl::Interface>                _features;
        api::feature::Service<>::Opposite   _featureService;

        //link
        api::link::Local<> _link;

        //rdbInstance
        api::rdb::Instance<> _rdbInstance;

        std::unique_ptr<node::NetEnumerator> _netEnumerator;
        node::TransportHub<transport::Acceptor<>, transport::acceptor::Downstream<>> _acceptors;
        node::TransportHub<transport::Connector<>, transport::connector::Downstream<>> _connectors;

        Set<transport::Address> _declaredLocalAddresses;

        transport::Natt<> _natt;

        struct Mapping
        {
            Node *                      _node {};
            transport::natt::Mapping<>  _api;
            sbs::Owner                  _sbsOwner;
            transport::Address          _external;

            Mapping(Node* node, transport::natt::Mapping<>&& api, const transport::Address& internal);
            ~Mapping();
        };

        std::map<transport::Address, Mapping> _nattMappings;

    private:
        Set<transport::Address>                                                 _connectionsInProgress;
        std::multimap<transport::Address, cmt::Promise<api::link::Remote<>>>    _joinWaiters;


    private:
        Map<idl::ILid, api::feature::AgentProvider<>> _agentRegistry;
    };
}
