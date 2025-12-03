// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "io.hpp"
#include "../peer.hpp"

namespace dci::module::ppn::discovery::peer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::Io(Peer* srv, const node::link::Id& id)
        : _srv{srv}
        , _id{id}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::~Io()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const transport::Address& Io::address() const
    {
        return _address;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::joined(const node::link::Remote<>& r)
    {
        r.involvedChanged() += _sol * [r=r.weak(),this](bool v)
        {
            if(!v)
            {
                _srv->free(r);
            }
        };

        r->remoteAddress().then() += _sol * [r=r.weak(),this](cmt::Future<transport::Address> in)
        {
            if(!in.resolvedValue())
            {
                _srv->free(r);
                return;
            }
            _address = in.detachValue();

            node::link::Remote<>{r}->getInstance(api::Supplier<>::lid()).then() += _sol * [r,this](cmt::Future<idl::Interface> in)
            {
                if(!in.resolvedValue())
                {
                    _srv->free(r);
                    return;
                }
                _supplierIn = in.detachValue();

                _supplierIn->put() += _sol * [this](transport::Address&& a)
                {
                    _srv->input(_address, _id, std::move(a));
                };

                _supplierIn.involvedChanged() += _sol * [r=r.weak(),this](bool v)
                {
                    if(!v)
                    {
                        _supplierIn.reset();
                        _srv->free(r);
                    }
                };

                regularGet();
            };
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::Supplier<> Io::getSupplierOut()
    {
        if(!_supplierOut)
        {
            _supplierOut.init();

            _supplierOut->get() += _sol * [this]
            {
                return cmt::readyFuture(_srv->output(_address));
            };
        }

        return _supplierOut.opposite();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::declared(const transport::Address& a)
    {
        if(_supplierOut)
        {
            _supplierOut->put(a);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::regularGet()
    {
        if(!_supplierIn)
        {
            return;
        }

        _supplierIn->get().then() += _sol * [this](cmt::Future<List<transport::Address>> in)
        {
            if(!in.resolvedValue()) return;
            for(transport::Address& a : in.detachValue())
            {
                _srv->input(_address, _id, std::move(a));
            }
        };
    }
}
