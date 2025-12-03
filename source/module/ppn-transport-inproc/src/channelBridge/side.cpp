// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channelBridge/side.hpp"
#include "channelBridge/pumper.hpp"

namespace dci::module::ppn::transport::inproc::channelBridge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Side::Side()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Side::~Side()
    {
        stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::start(Side* parther, const apit::Address& address)
    {
        dbgAssert(!_partner);
        _partner = parther;

        _ch->localAddress() += this * [=]{return cmt::readyFuture(address);};
        _ch->remoteAddress() += this * [=]{return cmt::readyFuture(address);};
        _ch->originalRemoteAddress() += this * [=]{return cmt::readyFuture(address);};

        _ch->output() += this * [this](Bytes&& data)
        {
            if(_partner)
            {
                _partner->input(std::move(data));
            }
            else
            {
                fail();
                close();
            }
        };

        _ch->unlockInput() += this * [this]()
        {
            _inputLocked = false;
            wantPump();
        };

        _ch->lockInput() += this * [this]()
        {
            _inputLocked = true;
        };

        _ch->close() += this * [this]()
        {
            _close = true;
            wantPump();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::stop()
    {
        unwantPump();

        _fail = true;
        _close = true;

        pump();

        if(_partner)
        {
            _partner->partnerGone();
            _partner = nullptr;
        }

        flush();
        _closed = true;
        _inputBuf.clear();
        _ch.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::fail()
    {
        if(!_fail)
        {
            _fail = true;
            wantPump();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::close()
    {
        if(!_close)
        {
            _close = true;
            wantPump();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const apit::Channel<>::Opposite& Side::ch() const
    {
        return _ch;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::pump()
    {
        if(_closed)
        {
            return;
        }

        Bytes inputBuf;
        bool emitInput{};
        bool emitFail{};
        bool emitClosed{};

        if(!_inputBuf.empty() && !_inputLocked)
        {
            inputBuf = std::move(_inputBuf);
            _inputBuf.clear();
            emitInput = true;
        }

        if(_fail)
        {
            _close = true;
            emitFail = true;
        }

        if(_close)
        {
            _closed = true;
            _inputBuf.clear();
            emitClosed = true;

            if(_partner)
            {
                _partner->partnerGone();
                _partner = nullptr;
            }
        }

        if(emitInput || emitFail || emitClosed)
        {
            apit::Channel<>::Opposite ch {_ch};

            if(emitInput)
            {
                ch->input(std::move(inputBuf));
            }
            if(emitFail)
            {
                ch->failed(exception::buildInstance<api::ConnectionLost>());
            }
            if(emitClosed)
            {
                ch->closed();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::wantPump()
    {
        dbgAssert(g_pumperPtr);
        if(g_pumperPtr)
        {
            g_pumperPtr->want(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::unwantPump()
    {
        dbgAssert(g_pumperPtr);
        if(g_pumperPtr)
        {
            g_pumperPtr->unwant(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::partnerGone()
    {
        if(_partner)
        {
            _partner = nullptr;
        }

        fail();
        close();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Side::input(Bytes&& data)
    {
        _inputBuf.end().write(std::move(data));
        wantPump();
    }
}
