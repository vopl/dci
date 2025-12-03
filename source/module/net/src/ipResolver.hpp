// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
