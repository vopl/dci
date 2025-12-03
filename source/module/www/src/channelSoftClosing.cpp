// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channelSoftClosing.hpp"

namespace dci::module::www
{
    namespace
    {
        class ChannelSoftClosingInstance
            : public ChannelSoftClosing
            , public mm::heap::Allocable<ChannelSoftClosingInstance>
        {
        };

        std::unique_ptr<ChannelSoftClosingInstance> p_instance{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChannelSoftClosing::push(api::stream::Channel<>&& target)
    {
        dbgAssert(target);

        auto [iter, emplaced] = _channels.emplace(std::move(target));
        dbgAssert(emplaced);
        if(emplaced)
        {
            const Channel& channel = *iter;
            channel._target->shutdown();

            {
                auto cleanup = [this, weakTarget = channel._target.weak()]
                {
                    auto iter = _channels.find(weakTarget);
                    if(_channels.end() != iter)
                        _channels.erase(iter);
                };

                channel._target.involvedChanged() += channel._sol * [cleanup](bool involved)
                {
                    if(!involved)
                        cleanup();
                };
                channel._target->closed() += channel._sol * cleanup;
                channel._target->failed() += channel._sol * [cleanup](ExceptionPtr&&)
                {
                    cleanup();
                };
                channel._timer.tick() += channel._sol * std::move(cleanup);
                channel._timer.start();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChannelSoftClosing::moduleStarted()
    {
        p_instance = std::make_unique<ChannelSoftClosingInstance>();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelSoftClosing& ChannelSoftClosing::instance()
    {
        dbgAssert(p_instance);
        return *p_instance;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChannelSoftClosing::moduleStopRequested()
    {
        // хост запросил добровольный останов модуля
        // пока ничего не делаем, пусть еще некоторое время каналы будут не закрыты, может за эту толику успеет еще что то отправиться в сеть
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChannelSoftClosing::moduleStopped()
    {
        p_instance.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelSoftClosing::ChannelSoftClosing()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelSoftClosing::~ChannelSoftClosing()
    {
    }
}
