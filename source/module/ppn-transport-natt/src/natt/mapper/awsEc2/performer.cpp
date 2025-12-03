// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "performer.hpp"
#include "lookout.hpp"

namespace dci::module::ppn::transport::natt::mapper::awsEc2
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::Performer(Lookout* l, const net::Ip4Endpoint& internal)
        : _l{l}
        , _internal{internal}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::~Performer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::start()
    {
        return fetch();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::keepalive()
    {
        if(!_l)
        {
            _revision = {};
            _external = {};
            return false;
        }

        if(_revision == _l->revision())
        {
            _l->changed().wait();
        }

        return fetch();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Performer::stop()
    {
        _revision = {};
        _external = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::fetch()
    {
        if(!_l)
        {
            _revision = {};
            _external = {};
            return false;
        }

        _revision = _l->revision();

        if(_l->internal() != _internal.address)
        {
            _external = {};
            return false;
        }

        _external.value = "tcp4://" + utils::ip::toString(_l->external().octets, _internal.port);
        return true;
    }
}
