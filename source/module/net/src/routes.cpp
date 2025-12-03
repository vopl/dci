// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "host.hpp"

namespace dci::module::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Routes::Routes(api::Host<>::Opposite* iface)
        : _iface(iface)
    {
        (*_iface)->route4() += this * [&]()
        {
            return cmt::readyFuture(_table4);
        };

        (*_iface)->route6() += this * [&]()
        {
            return cmt::readyFuture(_table6);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Routes::~Routes()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Routes::add(const api::route::Entry4& e)
    {
        _added4.push_back(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Routes::del(const api::route::Entry4& e)
    {
        _deleted4.push_back(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Routes::add(const api::route::Entry6& e)
    {
        _added6.push_back(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Routes::del(const api::route::Entry6& e)
    {
        _deleted6.push_back(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Routes::flushChanges()
    {
        List<api::route::Entry4> added4;
        List<api::route::Entry6> added6;

        List<api::route::Entry4> deleted4;
        List<api::route::Entry6> deleted6;

        added4.swap(_added4);
        added6.swap(_added6);

        deleted4.swap(_deleted4);
        deleted6.swap(_deleted6);

        auto doDelete = [](auto& container, const auto& element)
        {
            auto iter = std::find(container.begin(), container.end(), element);
            if(container.end() != iter)
            {
                container.erase(iter);
                return true;
            }

            return false;
        };

        bool some4Deleted = false;
        for(const api::route::Entry4& e : deleted4)
        {
            some4Deleted |= doDelete(_table4, e);
        }

        for(const api::route::Entry4& e : added4)
        {
            _table4.push_back(e);
        }

        bool some6Deleted = false;
        for(const api::route::Entry6& e : deleted6)
        {
            some6Deleted |= doDelete(_table6, e);
        }

        for(const api::route::Entry6& e : added6)
        {
            _table6.push_back(e);
        }

        if(!added4.empty() || some4Deleted)
        {
            (*_iface)->route4Changed();
        }

        if(!added6.empty() || some6Deleted)
        {
            (*_iface)->route6Changed();
        }
    }
}
