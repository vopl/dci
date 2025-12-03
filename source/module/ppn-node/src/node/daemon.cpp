// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "daemon.hpp"

namespace dci::module::ppn::node
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Daemon::Daemon()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Daemon::~Daemon()
    {
        _node.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Daemon::startImpl(idl::gen::Config&& config)
    {
        _node.reset(new Node);
        _node->start(std::move(config));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Daemon::stopImpl()
    {
        if(_node)
        {
            _node->stop();
            _node.reset();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    idl::Interface Daemon::serviceImpl()
    {
        return idl::Interface{_node->opposite()};
    }
}
