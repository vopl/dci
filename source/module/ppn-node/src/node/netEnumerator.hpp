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

namespace dci::module::ppn::node
{
    class NetEnumerator
        : public sbs::Owner
        , public mm::heap::Allocable<NetEnumerator>
    {
    public:
        struct Address
        {
            utils::ip::Scope _scope {};
            std::string      _value;

            bool operator <(const Address& v) const
            {
                return std::tie(_scope, _value) < std::tie(v._scope, v._value);
            }
        };

    public:
        NetEnumerator();
        ~NetEnumerator();

        void start();

        sbs::Signal<void, ExceptionPtr> failed();
        sbs::Signal<void, Address> add();
        sbs::Signal<void, Address> del();

    private:
        sbs::Wire<void, ExceptionPtr> _failed;
        sbs::Wire<void, Address> _add;
        sbs::Wire<void, Address> _del;

        void addLink(uint32 id, net::Link<> link);
        void updateLink(uint32 id, net::Link<> link);
        void delLink(uint32 id);

        void updateResult();

    private:
        void spawn(auto mptr, auto... args);

    private:
        cmt::task::Owner _taskOwner;

    private:
        using Addresses = Set<Address>;
        Map<uint32, Addresses> _linkAddresses;

    private:
        Addresses   _result;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void NetEnumerator::spawn(auto mptr, auto ... args)
    {
        cmt::spawn() += _taskOwner * [=,this]() mutable
        {
            try
            {
                (this->*mptr)(args...);
            }
            catch(const cmt::task::Stop&)
            {
                //ignore
            }
            catch(...)
            {
                _failed.in(std::current_exception());
            }
        };
    }
}
