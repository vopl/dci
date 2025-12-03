// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "consumer.hpp"
#include "../io.hpp"

namespace dci::module::ppn::topology::lis::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Supplier::Supplier(Io* io)
        : _io{io}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Supplier::~Supplier()
    {
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner& Supplier::sbsOwner()
    {
        return _sbsOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Supplier::alloc(const api::Supplier<>& s, const node::link::Remote<>& r)
    {
        _sbsOwner.flush();

        r->remoteAddress().then() += _sbsOwner * [this](cmt::Future<transport::Address> in)
        {
            if(in.resolvedValue())
            {
                _address = in.detachValue();
            }
            else
            {
                _address = {};
            }
        };

        _remote = s;
        _gridKernelSent = false;

        _remote->supply() += _sbsOwner * [this](const space::Id& id, List<transport::Address>&& as)
        {
            _io->supplied(_address, id, std::move(as));
        };

        _remote->subscribeNears();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Supplier::free(const api::Supplier<>& s)
    {
        if(_remote == s)
        {
            _sbsOwner.flush();
            _remote.reset();
            _address = {};
            _gridKernelSent = false;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Supplier::empty() const
    {
        return !_remote;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const transport::Address& Supplier::address() const
    {
        return _address;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Supplier::gridRequest(const grid::Kernel& gridKernel, const List<api::GridFillingStep>& filling)
    {
        dbgAssert(_remote);

        if(!_remote)
        {
            return;
        }

        if(!_gridKernelSent || _gridKernel != gridKernel)
        {
            _gridKernel = gridKernel;
            _gridKernelSent = true;
            _remote->gridSetup(_gridKernel.bits(), _gridKernel.size());
        }

        _remote->gridRequest(filling);
    }
}
