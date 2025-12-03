// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "performer.hpp"
#include "service.hpp"
#include "../../addr.hpp"

namespace dci::module::ppn::transport::natt::mapper::igdp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::Performer(Service* srv, api::Protocol protocol, uint16 internalPort, const net::IpEndpoint& externalEp)
        : _srv{srv}
        , _protocol{protocol}
        , _internalPort{internalPort}
        , _externalEp{externalEp}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::~Performer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::start()
    {
        bool res = _srv && _srv->map(_externalEp, _internalPort, _protocol, std::chrono::seconds{60*10});
        if(res)
        {
            _external.value = addr::toString(_externalEp, _protocol);
        }
        else
        {
            _external.value.clear();
        }
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::keepalive()
    {
        if(!_srv) return false;
        poll::WaitableTimer t{std::chrono::seconds{60*1}};
        t.start();
        cmt::waitAny(t.waitable(), _srv->serviceChangedWaitable());
        return start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Performer::stop()
    {
        if(_srv && !_external.value.empty())
        {
            _srv->map(_externalEp, _internalPort, _protocol, std::chrono::seconds{});
            _external.value.clear();
        }
    }
}
