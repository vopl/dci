// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "remoteEdge.hpp"
#include "protocol.hpp"

namespace dci::module::stiac
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RemoteEdge::RemoteEdge(Protocol* protocol, const api::RemoteEdge<>::Opposite& interface)
        : stages::Base(protocol)
        , _interface(interface)
    {
        _interface.involvedChanged() += this * [this](bool v)
        {
            if(!v)
            {
                _protocol->remoteEdgeWantRemove(this);
            }
        };

        //пользователь -> цепь, увести в следующий линк
        _interface->input() += this * [this](Bytes&& data)
        {
            Base::accumulateOutput(std::move(data));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RemoteEdge::~RemoteEdge()
    {
        sbs::Owner::flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RemoteEdge::input(Bytes&& msg)
    {
        //цепь -> пользователь
        _interface->output(std::move(msg));
    }
}
