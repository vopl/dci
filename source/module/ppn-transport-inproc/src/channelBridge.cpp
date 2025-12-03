// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channelBridge.hpp"

namespace dci::module::ppn::transport::inproc
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<apit::Channel<>, apit::Channel<>> ChannelBridge::allocate(const apit::Address& address)
    {
        ChannelBridge* cb = new ChannelBridge(address);

        cb->_s1.ch().involvedChanged() += cb->_s1 * [cb](bool v)
        {
            if(!v)
            {
                cb->_s1.stop();

                if(!(--cb->_useCounter))
                {
                    delete cb;
                }
            }
        };

        cb->_s2.ch().involvedChanged() += cb->_s2 * [cb](bool v)
        {
            if(!v)
            {
                cb->_s2.stop();

                if(!(--cb->_useCounter))
                {
                    delete cb;
                }
            }
        };

        return std::make_pair(cb->_s1.ch().opposite(), cb->_s2.ch().opposite());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelBridge::ChannelBridge(const apit::Address& address)
    {
        _s1.start(&_s2, address);
        _s2.start(&_s1, address);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelBridge::~ChannelBridge()
    {
        _s1.stop();
        _s2.stop();
    }
}
