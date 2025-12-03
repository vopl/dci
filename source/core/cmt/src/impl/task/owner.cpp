// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "owner.hpp"
#include "../scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::Owner()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::~Owner()
    {
        stopImpl(false, true);

        _tasks.each([](Body* task)
        {
            task->detachOwner();
        });
        dbgAssert(_tasks.empty());

        dbgAssert(_waiting.empty());
        _waiting.each([](Body* task)
        {
            task->detachOwner();
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::subscribe(Body* task)
    {
        dbgAssert(!_tasks.contains(task));
        _tasks.push(task);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::unsubscribe(Body* task)
    {
        if(_waitingActive)
        {
            if(_waiting.contains(task))
                _waiting.remove(task);
            else if(_tasks.contains(task))
                _tasks.remove(task);
            else
                std::unreachable();
        }
        else
        {
            dbgAssert(_waiting.empty());
            dbgAssert(_tasks.contains(task));
            _tasks.remove(task);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Owner::stopRequested() const
    {
        return _stopRequested;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Owner::empty() const
    {
        return _tasks.empty() && _waiting.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::flush(bool andWait)
    {
        return stopImpl(true, andWait);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::stop(bool andWait)
    {
        return stopImpl(false, andWait);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::wait()
    {
        if(_waitingActive)
            return;
        _waitingActive = true;

        Scheduler& scheduler = Scheduler::instance();
        Body* currentTask = scheduler.currentTask();

        std::size_t prevCount{};
        for(;;)
        {
            std::size_t count{};
            while(!_tasks.empty())
            {
                Body* task = _tasks.first();
                _tasks.remove(task);
                _waiting.push(task);
                ++count;

                if(currentTask != task)
                    scheduler.switchTo(task);
            }
            _tasks = std::move(_waiting);

            if(std::exchange(prevCount, count) == count)
                break;
        }

        dbgAssert(_waitingActive);
        _waitingActive = false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::stopImpl(bool once, bool andWait)
    {
        _stopRequested = true;
        _tasks.each([](Body* task)
        {
            task->stop(false);
        });

        if(andWait)
            wait();

        if(once)
            _stopRequested = false;
    }
}
