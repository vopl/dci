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
#include "ipResolver/task.hpp"

namespace dci::module::net
{
    class Host;

    class IpResolver
        : public sbs::Owner
    {
    public:
        IpResolver(api::Host<>::Opposite* iface);
        ~IpResolver();

    private:
        template <class Value>
        auto execute(auto&& endpoint);

        void ensureWorkersRan();
        void workerProc();

    private:
        api::Host<>::Opposite *     _iface = nullptr;
        std::vector<std::thread>    _workers;

        ipResolver::Task*           _tasks4Worker = nullptr;

        std::mutex                  _mtx;
        std::condition_variable     _notifier4Worker;

        bool                        _stopFlag = false;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Value>
    auto IpResolver::execute(auto&& endpoint)
    {
        if(endpoint.empty())
        {
            return cmt::readyFuture(Value{});
        }

        ensureWorkersRan();

        using namespace ipResolver;

        auto* t = new Task;
        auto res = t->init<Value>(std::forward<decltype(endpoint)>(endpoint));

        {
            std::unique_lock l(_mtx);

            t->_next4Worker = _tasks4Worker;
            _tasks4Worker = t;
        }

        _notifier4Worker.notify_one();

        return res;
    }
}
