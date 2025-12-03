// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "performer.hpp"
#include "lookout.hpp"

namespace dci::module::ppn::transport::natt::mapper::custom
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::Performer(Lookout* l, const apit::Address& internal)
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

        const auto& maps = _l->maps();
        auto iter = maps.find(_internal);
        if(maps.end() == iter)
        {
            _external = {};
            return false;
        }

        _external = iter->second;
        return true;
    }

}
