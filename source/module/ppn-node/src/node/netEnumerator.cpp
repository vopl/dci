// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "netEnumerator.hpp"

extern dci::host::module::Entry* dciModuleEntry;

namespace dci::module::ppn::node
{
    using namespace dci::idl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    NetEnumerator::NetEnumerator()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    NetEnumerator::~NetEnumerator()
    {
        _linkAddresses.clear();
        updateResult();

        flush();
        _taskOwner.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void NetEnumerator::start()
    {
        net::Host<> netHost = dciModuleEntry->manager()->createService<net::Host<>>().value();

        netHost->linkAdded() += this * [this](uint32 id, net::Link<> link)
        {
            spawn(&NetEnumerator::addLink, id, link);
        };

        auto links = netHost->links().value();
        for(auto& p : links)
        {
            addLink(p.first, p.second);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, ExceptionPtr> NetEnumerator::failed()
    {
        return _failed.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, NetEnumerator::Address> NetEnumerator::add()
    {
        return _add.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, NetEnumerator::Address> NetEnumerator::del()
    {
        return _del.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void NetEnumerator::addLink(uint32 id, net::Link<> link)
    {
        link->removed() += this * [id,this]
        {
            delLink(id);
        };

        link.involvedChanged() += this * [id,this](bool v)
        {
            if(!v)
            {
                delLink(id);
            }
        };

        link->changed() += this * [link=link.weak(),id,this]
        {
            spawn(&NetEnumerator::updateLink, id, link);
        };

        updateLink(id, link);
    }

    namespace
    {
        NetEnumerator::Address addrCnvt(const net::link::Address& src)
        {
            NetEnumerator::Address res;

            if(src.holds<net::link::Ip4Address>())
            {
                const net::Ip4Address& ip4 = src.get<net::link::Ip4Address>().address;
                res._scope = utils::ip::scope(ip4.octets);
                res._value = utils::ip::toString(ip4.octets);
            }
            else //if(src.holds<net::link::Ip6Address>())
            {
                dbgAssert(src.holds<net::link::Ip6Address>());

                const net::Ip6Address& ip6 = src.get<net::link::Ip6Address>().address;
                res._scope = utils::ip::scope(ip6.octets);
                res._value = utils::ip::toString(ip6.octets, ip6.linkId);
            }

            return res;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void NetEnumerator::updateLink(uint32 id, net::Link<> link)
    {
        Addresses& dst = _linkAddresses[id];
        dst.clear();

        net::link::Flags flags = link->flags().value();
        if((flags & net::link::Flags::up) && (flags & net::link::Flags::running))
        {
            List<net::link::Address> src = link->addr().value();
            for(const net::link::Address& la : src)
            {
                dst.insert(addrCnvt(la));
            }
        }

        updateResult();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void NetEnumerator::delLink(uint32 id)
    {
        _linkAddresses.erase(id);

        updateResult();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void NetEnumerator::updateResult()
    {
        Set<Address> addrs;

        for(const auto& p : _linkAddresses)
        {
            addrs.insert(p.second.begin(), p.second.end());
        }

        List<Address> toAdd, toDel;

        std::set_difference(addrs.begin(), addrs.end(),
                            _result.begin(), _result.end(),
                            std::inserter(toAdd, toAdd.end()));

        std::set_difference(_result.begin(), _result.end(),
                            addrs.begin(), addrs.end(),
                            std::inserter(toDel, toDel.end()));

        _result = addrs;

        for(Address& a : toDel)
        {
            _del.in(std::move(a));
        }

        for(Address a : toAdd)
        {
            _add.in(std::move(a));
        }
    }

}
