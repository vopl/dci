// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "online.hpp"
#include "../io.hpp"

namespace dci::module::ppn::topology::lis::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Online::Online(Io* io, const space::Id& id)
        : _io{io}
        , _id{id}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Online::~Online()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const space::Id& Online::id() const
    {
        return _id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::joined(const node::link::Remote<>& r)
    {
        bool wasCharged = charged();

        _remotes.insert(r);

        if(!wasCharged && charged())
        {
            _io->charged(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::disjoined(const node::link::Remote<>& r)
    {
        _remotes.erase(r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::state(real64 rating, List<transport::Address>&& addresses)
    {
        bool wasCharged = charged();

        _rating = rating;

        List<transport::Address> prevAddresses{std::move(addresses)};
        _addresses.swap(prevAddresses);

        if(charged())
        {
            if(!wasCharged)
            {
                _io->charged(this);
            }
            else if(_addresses != prevAddresses)
            {
                _io->changed(this);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Online::empty() const
    {
        return _remotes.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Online::charged() const
    {
        return _rating > 0 && !_addresses.empty() && !_remotes.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const List<transport::Address>& Online::addresses() const
    {
        return _addresses;
    }
}
