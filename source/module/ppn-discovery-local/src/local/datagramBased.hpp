// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../local.hpp"

#define LLOGW(msg) LOGW(this->derived().tag()<<": "<<msg)
#define LLOGI(msg) LOGI(this->derived().tag()<<": "<<msg)

namespace dci::module::ppn::discovery::local
{
    template <class Derived, class Interface>
    class DatagramBased
        : public Local<Derived, Interface>
    {
        using Base = Local<Derived, Interface>;

    public:
        DatagramBased(host::Manager* hostManager);
        ~DatagramBased();

        void started();
        void stopped();
        void declared(const Entry& entry);
        void undeclared(const Entry& entry);

    protected:
        bool isMaster() const;
        const net::datagram::Channel<>& net();

    protected:
        Bytes makeMessage();
        bool parseAndEmitMessage(Bytes&& msg);

    private:
        bool initNetMaster();
        bool initNetSlave();

    private:
        void netReset();

    private:
        host::Manager*                      _hostManager;
        cmt::Notifier                       _action;
        poll::WaitableTimer<>               _smoothedAction{std::chrono::milliseconds{200}, false};
        poll::WaitableTimer<cmt::Pulser>    _failQuarantine{std::chrono::seconds{10}, false};
        bool                                _isMaster{false};
        net::datagram::Channel<>            _net;
        sbs::Owner                          _netSbsOwner;
        cmt::task::Owner                    _workerOwner;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    DatagramBased<Derived, Interface>::DatagramBased(host::Manager* hostManager)
        : _hostManager(hostManager)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    DatagramBased<Derived, Interface>::~DatagramBased()
    {
        netReset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void DatagramBased<Derived, Interface>::started()
    {
        cmt::spawn() += _workerOwner * [this]
        {
            _isMaster = true;

            poll::WaitableTimer<> ticker {std::chrono::seconds{1*60} + (rand()%60000)*std::chrono::milliseconds{1}, true};
            ticker.start();
            _action.raise();

            for(;;)
            {
                try
                {
                    cmt::waitAny(_action, _smoothedAction, ticker);
                    if(!this->isStarted())
                    {
                        netReset();
                        return;
                    }

                    if(_failQuarantine.started())
                    {
                        _failQuarantine.wait();
                    }

                    if(!_net)
                    {
                        _isMaster = initNetMaster();
                    }

                    if(!_net && !_isMaster)
                    {
                        initNetSlave();
                    }

                    if(_net)
                    {
                        this->derived().tick();
                    }
                }
                catch(const cmt::task::Stop&)
                {
                    return;
                }
                catch(...)
                {
                    _failQuarantine.start();
                }
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void DatagramBased<Derived, Interface>::stopped()
    {
        _workerOwner.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void DatagramBased<Derived, Interface>::declared(const Entry&)
    {
        if(_net)
        {
            _smoothedAction.start();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void DatagramBased<Derived, Interface>::undeclared(const Entry&)
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool DatagramBased<Derived, Interface>::isMaster() const
    {
        return _isMaster;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    const net::datagram::Channel<>& DatagramBased<Derived, Interface>::net()
    {
        return _net;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    Bytes DatagramBased<Derived, Interface>::makeMessage()
    {
        Bytes res;
        stiac::serialization::Arch(res.begin()) << this->linkId() << this->entriesOut();
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool DatagramBased<Derived, Interface>::parseAndEmitMessage(Bytes&& msg)
    {
        link::Id linkId;
        Entries entries;

        try
        {
            stiac::serialization::Arch arch(msg.begin());
            arch >> linkId >> entries;
        }
        catch(...)
        {
            LLOGI("bad message received: "<<exception::currentToString());
            return 0;
        }

        if(linkId != this->linkId())
        {
            return 0 < this->emitEntries(entries);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool DatagramBased<Derived, Interface>::initNetMaster()
    {
        dbgAssert(!_net);

        try
        {
            idl::gen::net::Host<> netHost = _hostManager->createService<idl::gen::net::Host<>>().value();
            _net = netHost->datagramChannel().value();
            _net->bind(this->derived().masterPoint()).value();
            _net->setOption(idl::gen::net::option::Broadcast{true}).value();
        }
        catch(const cmt::task::Stop&)
        {
            throw;
        }
        catch(const net::AddressInIse&)
        {
            _failQuarantine.start();

            //ignore
            netReset();
            return false;
        }
        catch(...)
        {
            _failQuarantine.start();

            LLOGW("init master failed: "<<exception::currentToString());
            netReset();
            return false;
        }

        _net->received() += _netSbsOwner * [this](Bytes&& data, idl::gen::net::Endpoint&& ep)
        {
            LLOGI("master received "<<data.size()<<" bytes");

            if(parseAndEmitMessage(std::move(data)))
            {
                Bytes msg = makeMessage();
                LLOGI("master reply "<<msg.size()<<" bytes");

                _net->send(std::move(msg), std::move(ep));
            }
        };

        _net->failed() += _netSbsOwner * [this](ExceptionPtr e)
        {
            LLOGW("master failed: "<<exception::toString(e));

            _failQuarantine.start();

            try
            {
                std::rethrow_exception(e);
            }
            catch(const idl::gen::net::NetworkUnreachable&)
            {
                //ignore
                return;
            }
            catch(const idl::gen::net::UnavaliableTryAgain&)
            {
                //ignore
                return;
            }
            catch(...)
            {
            }

            netReset();
            _action.raise();
        };

        _net->closed() += _netSbsOwner * [this]()
        {
            LLOGI("master closed");
            _failQuarantine.start();
            netReset();
            _action.raise();
        };

        LLOGI("master opened");
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool DatagramBased<Derived, Interface>::initNetSlave()
    {
        dbgAssert(!_net);

        try
        {
            _net = _hostManager->createService<idl::gen::net::Host<>>().value()->datagramChannel().value();
            _net->bind(this->derived().slavePoint()).value();
            _net->setOption(idl::gen::net::option::Broadcast{true}).value();
        }
        catch(const cmt::task::Stop&)
        {
            throw;
        }
        catch(...)
        {
            LLOGW("init slave failed: "<<exception::currentToString());
            _failQuarantine.start();
            netReset();
            return false;
        }

        _net->received() += _netSbsOwner * [this](Bytes&& data, const idl::gen::net::Endpoint& ep)
        {
            LLOGI("slave received "<<data.size()<<" bytes");

            if(parseAndEmitMessage(std::move(data)))
            {
                Bytes msg = makeMessage();
                LLOGI("slave reply "<<msg.size()<<" bytes");

                _net->send(std::move(msg), std::move(ep));
            }
        };

        _net->failed() += _netSbsOwner * [this](ExceptionPtr e)
        {
            LLOGW("slave failed: "<<exception::toString(e));

            _failQuarantine.start();

            try
            {
                std::rethrow_exception(e);
            }
            catch(const idl::gen::net::NetworkUnreachable&)
            {
                //ignore
                return;
            }
            catch(const idl::gen::net::UnavaliableTryAgain&)
            {
                //ignore
                return;
            }
            catch(...)
            {
            }

            netReset();
            _action.raise();
        };

        _net->closed() += _netSbsOwner * [this]()
        {
            LLOGI("slave closed");
            _failQuarantine.start();
            netReset();
            _action.raise();
        };

        LLOGI("slave opened");
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void DatagramBased<Derived, Interface>::netReset()
    {
        LLOGI((_isMaster?"master":"slave")<<" reset");

        _netSbsOwner.flush();
        _net.reset();
    }

}
