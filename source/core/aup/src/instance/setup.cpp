// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/aup/instance/setup.hpp>
#include <dci/aup/exception.hpp>
#include "../instance.hpp"

namespace dci::aup::instance::setup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void start(const std::vector<std::string>& args)
    {
        if(g_instance)
        {
            throw aup::Exception{"instance already initialized"};
        }

        g_instance.reset(new Instance);

        try
        {
            g_instance->start(args);
        }
        catch(...)
        {
            g_instance.reset();
            throw;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool targetComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->targetComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void updateTarget()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->updateTarget();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void collectGarbage()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->collectGarbage();
    }
}
