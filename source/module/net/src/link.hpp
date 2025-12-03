// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net
{
    class Link
        : public api::Link<>::Opposite
        , public sbs::Owner
        , public mm::heap::Allocable<Link>
    {
    public:
        Link(uint32 id);
        ~Link();

        void setHwAddress(const List<uint8>& v);
        void setName(const std::string& v);
        void setMtu(uint32 v);
        void setFlags(api::link::Flags v);

        void setIp4(List<api::link::Ip4Address> addresses);
        void addIp4(api::link::Ip4Address address);
        void delIp4(api::link::Ip4Address address);

        void setIp6(List<api::link::Ip6Address> addresses);
        void addIp6(api::link::Ip6Address address);
        void delIp6(api::link::Ip6Address address);

        void flushChanges();
        void remove();

    private:
        uint32                      _id = 0;
        List<uint8>                 _hwAddress;
        String                      _name;
        uint32                      _mtu = 0;
        api::link::Flags            _flags {};
        List<api::link::Ip4Address> _ip4;
        List<api::link::Ip6Address> _ip6;

        bool                        _changed = false;
    };
}
