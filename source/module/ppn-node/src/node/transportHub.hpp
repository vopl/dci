/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "netEnumerator.hpp"
#include "utils.hpp"

namespace dci::module::ppn::node
{
    template <class Hi, class Lo>
    class TransportHub
        : public sbs::Owner
    {
    public:
        using LoMaker = std::function<Lo(const transport::Address&)>;
        using AddressFixer = std::function<transport::Address(const transport::Address&)>;

    public:
        TransportHub();
        ~TransportHub();

        void start(
                Hi&& hi,
                AddressFixer addressFixer,
                LoMaker loMaker,
                const auto& conf,
                const auto& netEnumeratorProvider);
        void stop();

        Hi hi() const;

        sbs::Signal<void, transport::Address> loAdded();
        sbs::Signal<void, transport::Address> loDeleted();

    private:
        void autoConf(
                const auto& conf,
                const auto& netEnumeratorProvider);

        void autoConfIp(
                const auto& conf,
                const auto& netEnumeratorProvider,
                uint32 scope);

    private:
        template <class I> void addLo(const std::pair<I,I>& range);
        void addLo(transport::Address&& a);
        void delLo(transport::Address&& a);

    private:
        Hi  _hi;

        sbs::Wire<void, transport::Address> _loAdded;
        sbs::Wire<void, transport::Address> _loDeleted;

        struct LoInstance
        {
            size_t  _useCounter = 0;
            Lo      _lo;
        };

        Map<transport::Address, LoInstance> _loInstances;

        AddressFixer    _addressFixer;
        LoMaker         _loMaker;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    TransportHub<Hi, Lo>::TransportHub()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    TransportHub<Hi, Lo>::~TransportHub()
    {
        stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    void TransportHub<Hi, Lo>::start(
            Hi&& hi,
            AddressFixer addressFixer,
            LoMaker loMaker,
            const auto& conf,
            const auto& netEnumeratorProvider)
    {
        _hi = std::move(hi);
        _addressFixer = addressFixer;
        _loMaker = loMaker;

        autoConf(conf, netEnumeratorProvider);

        addLo(conf.equal_range("custom"));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    void TransportHub<Hi, Lo>::stop()
    {
        flush();

        if(_hi)
        {
            for(const auto&[a, i]: _loInstances)
            {
                if(i._lo)
                {
                    _hi->del(i._lo);
                    _loDeleted.in(a);
                }
            }
        }

        _loInstances.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    Hi TransportHub<Hi, Lo>::hi() const
    {
        return _hi;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    sbs::Signal<void, transport::Address> TransportHub<Hi, Lo>::loAdded()
    {
        return _loAdded.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    sbs::Signal<void, transport::Address> TransportHub<Hi, Lo>::loDeleted()
    {
        return _loDeleted.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    void TransportHub<Hi, Lo>::autoConf(
            const auto& conf,
            const auto& netEnumeratorProvider)
    {
        if(conf.get("inproc", true))
        {
            addLo(_addressFixer(transport::Address{"inproc://%auto%"}));
        }

        if(conf.get("local", true))
        {
            addLo(_addressFixer(transport::Address{"local://%auto%"}));
        }

        if(conf.get("ip4", true))
        {
            autoConfIp(conf.get_child("ip4", decltype(conf){}), netEnumeratorProvider, static_cast<uint32>(dci::utils::ip::Scope::ip4));
        }

        if(conf.get("ip6", true))
        {
            autoConfIp(conf.get_child("ip6", decltype(conf){}), netEnumeratorProvider, static_cast<uint32>(dci::utils::ip::Scope::ip6));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    void TransportHub<Hi, Lo>::autoConfIp(const auto& conf,
            const auto& netEnumeratorProvider,
            uint32 scope)
    {
        std::string port = conf.get("port", "");

        uint32 scopes = 0;
        if(conf.get("host", true)) scopes |= static_cast<uint32>(dci::utils::ip::Scope::host);
        if(conf.get("link", true)) scopes |= static_cast<uint32>(dci::utils::ip::Scope::link);
        if(conf.get("lan" , true)) scopes |= static_cast<uint32>(dci::utils::ip::Scope::lan );
        if(conf.get("wan" , true)) scopes |= static_cast<uint32>(dci::utils::ip::Scope::wan );

        auto filter = [=](const NetEnumerator::Address& a, transport::Address& ta)
        {
            if(!(static_cast<uint32>(a._scope) & scope))
            {
                return false;
            }

            if(!(static_cast<uint32>(a._scope) & scopes))
            {
                return false;
            }

            if(static_cast<uint32>(a._scope) & static_cast<uint32>(dci::utils::ip::Scope::ip4))
            {
                ta.value = "tcp4://" + a._value + (port.empty() ? port : ":"+port);
            }
            else if(static_cast<uint32>(a._scope) & static_cast<uint32>(dci::utils::ip::Scope::ip6))
            {
                ta.value = "tcp6://[" + a._value + "]" + (port.empty() ? port : ":"+port);
            }
            else
            {
                dbgFatal("never here");
                ta.value = "tcp://" + a._value + (port.empty() ? port : ":"+port);
            }

            return true;
        };

        NetEnumerator& neEnumerator = netEnumeratorProvider();
        neEnumerator.add() += this * [=,this](const NetEnumerator::Address& a)
        {
            transport::Address ta;
            if(filter(a, ta)) addLo(std::move(ta));
        };

        neEnumerator.del() += this * [=,this](const NetEnumerator::Address& a)
        {
            transport::Address ta;
            if(filter(a, ta)) delLo(std::move(ta));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    template <class I>
    void TransportHub<Hi, Lo>::addLo(const std::pair<I,I>& range)
    {
        for(auto iter(range.first); iter!=range.second; ++iter)
        {
            std::string addr = iter->second.data();

            if(!dci::utils::uri::valid(addr))
            {
                throw api::Error("bad address value in config: "+addr);
            }

            addLo(transport::Address{addr});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    void TransportHub<Hi, Lo>::addLo(transport::Address&& a)
    {
        LoInstance& i = _loInstances[a];
        i._useCounter++;
        if(!i._lo)
        {
            try
            {
                i._lo = _loMaker(a);
                if(i._lo)
                {
                    i._lo.involvedChanged() += this * [a2=a,this](bool v) mutable
                    {
                        if(!v)
                        {
                            delLo(std::move(a2));
                        }
                    };
                    _hi->add(i._lo);
                    _loAdded.in(a);
                }
            }
            catch(...)
            {
                std::rethrow_exception(
                            exception::buildInstance<api::Error>(std::current_exception(), "unable to use address: "+a.value)
                            );
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Hi, class Lo>
    void TransportHub<Hi, Lo>::delLo(transport::Address&& a)
    {
        auto iter = _loInstances.find(a);
        if(_loInstances.end() == iter)
        {
            return;
        }

        LoInstance& i = iter->second;
        dbgAssert(i._useCounter>0);
        if(i._useCounter>1)
        {
            i._useCounter--;
            return;
        }

        if(i._lo)
        {
            _hi->del(i._lo);
            _loDeleted.in(a);
        }
        _loInstances.erase(iter);
    }
}
