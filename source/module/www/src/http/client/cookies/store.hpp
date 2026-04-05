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

namespace dci::module::www::http::client::cookies
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Store
    {
    public:
        Store(const api::http::client::cookies::Store<>::Opposite& api, sbs::Owner& sol);
        ~Store();

    public:
        void set(api::http::client::cookies::Entry&& entry);
        void set(List<api::http::client::cookies::Entry>&& entries);
        List<api::http::client::cookies::Entry> get(api::http::client::cookies::entry::Id&& pattern);
        void del(api::http::client::cookies::entry::Id&& pattern);
        void expire(uint64 now, bool finalizeSession);
        Bytes serialize();
        ExceptionPtr deserialize(Bytes&& blob);

    public:
        void traverse(auto&& visitor);

        struct Notify
        {
            enum Kind
            {
                added,
                changed,
                deleted
            } _kind{};
            api::http::client::cookies::Entry _entry;
        };

        void add4Notify(Notify::Kind notifyKind, auto&& entry);
        void flushNotifies();

    private:
        List<api::http::client::cookies::Entry> _entries;

        using Watcher = api::http::client::cookies::store::Notifies<idl::interface::Side::opposite>;
        std::set<Watcher> _watchers;

        List<Notify> _notifies;
        poll::Timer _notifyActivator{{}, false, [this]{ flushNotifies(); }};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::traverse(auto&& visitor)
    {
        for(api::http::client::cookies::Entry& entry : _entries)
            visitor(entry);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Store::add4Notify(Notify::Kind notifyKind, auto&& entry)
    {
        if(_watchers.empty())
        {
            _notifies.clear();
            return;
        }

        _notifies.emplace_back(notifyKind, std::forward<decltype(entry)>(entry));
        _notifyActivator.start();
    }
}
