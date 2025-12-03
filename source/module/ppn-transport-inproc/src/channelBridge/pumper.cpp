// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "pumper.hpp"
#include "side.hpp"

namespace dci::module::ppn::transport::inproc::channelBridge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pumper::Pumper()
    {
        cmt::spawn() += _workerOwner * [this]
        {
            worker();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pumper::~Pumper()
    {
        dbgAssert(_wants.empty());
        _workerOwner.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pumper::want(Side* s)
    {
        _wants.insert(s);
        _workerWaker.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pumper::unwant(Side* s)
    {
        _wants.erase(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pumper::worker()
    {
        for(;;)
        {
            while(!_wants.empty())
            {
                Side* s = *_wants.begin();
                _wants.erase(_wants.begin());
                s->pump();
            }

            try
            {
                _workerWaker.wait();
            }
            catch(const cmt::task::Stop&)
            {
                dbgAssert(_wants.empty());

                while(!_wants.empty())
                {
                    Side* s = *_wants.begin();
                    _wants.erase(_wants.begin());

                    s->close();
                    s->pump();
                }

                break;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PumperPtr g_pumperPtr {};
}
