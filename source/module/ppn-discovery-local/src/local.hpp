// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::discovery::local
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct Entry
    {
        link::Id            _id;
        transport::Address  _address;

        friend bool operator<(const Entry& a, const Entry& b);
        friend void save(auto& ar, const Entry& v);
        friend void load(auto& ar, Entry& v);
    };
    using Entries = Set<Entry>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline bool operator<(const Entry& a, const Entry& b)
    {
        return std::tie(a._id, a._address) < std::tie(b._id, b._address);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void save(auto& ar, const Entry& v)
    {
        ar << v._id << v._address;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void load(auto& ar, Entry& v)
    {
        ar >> v._id >> v._address;
    }
}

namespace dci::module::ppn::discovery
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    class Local
        : public Interface
        , public host::module::ServiceBase<Derived>
    {
    public:
        Local();
        ~Local();

        bool isStarted() const;
        const link::Id& linkId() const;
        const local::Entries& entriesOut() const;

    protected:
        void declareImpl(auto&& entry);
        void undeclareImpl(auto&& entry);

    protected:
        Derived& derived();
        void started();
        void stopped();
        bool addressAllowed(const transport::Address& a);
        void declared(const local::Entry& entry);
        void undeclared(const local::Entry& entry);

        bool emitEntry(const local::Entry& entry, bool force = false);
        size_t emitEntries(const local::Entries& entries, bool force = false);

    private:
        bool                                _started {false};
        node::feature::RemoteAddressSpace<> _ras;
        link::Id                            _linkId {};

    private:
        local::Entries _entriesOut;

    private:
        local::Entries  _entriesIn;
        poll::Timer     _entriesInCleanTicker{std::chrono::seconds{30}, true, [this]{_entriesIn.clear();}};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    Local<Derived, Interface>::Local()
        : Interface{idl::interface::Initializer{}}
    {
        //link
        {
            link::Feature<>::Opposite op = *this;

            //in setup(feature::Service);
            op->setup() += this->serviceSol() * [this](link::feature::Service<> srv)
            {
                srv->id().then() += this->serviceSol() * [this](auto in)
                {
                    if(in.resolvedValue())
                    {
                        _linkId = in.value();
                    }
                };
            };
        }

        //node
        {
            node::Feature<>::Opposite op = *this;

            //in setup(feature::Service);
            op->setup() += this->serviceSol() * [this](node::feature::Service<> srv)
            {
                _ras =srv;

                //out start();
                srv->start() += this->serviceSol() * [this]
                {
                    if(_started)
                    {
                        return;
                    }
                    _started = true;
                    _entriesInCleanTicker.start();

                    this->derived().started();
                };

                //out stop();
                srv->stop() += this->serviceSol() * [this]
                {
                    if(!_started)
                    {
                        return;
                    }
                    _started = false;
                    _entriesInCleanTicker.stop();

                    this->derived().stopped();
                };

                srv->declared() += this->serviceSol() * [this](const transport::Address& a)
                {
                    declareImpl(local::Entry{_linkId, a});
                };

                srv->undeclared() += this->serviceSol() * [this](const transport::Address& a)
                {
                    undeclareImpl(local::Entry{_linkId, a});
                };

                srv->getDeclared().then(this->serviceSol(), [this](cmt::Future<Set<transport::Address>> in)
                {
                    if(in.resolvedValue())
                    {
                        for(const transport::Address& a : in.value())
                        {
                            declareImpl(local::Entry{_linkId, a});
                        }
                    }
                });
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    Local<Derived, Interface>::~Local()
    {
        this->serviceSol().flush();
        _started = false;
        _entriesInCleanTicker.stop();
        this->derived().stopped();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool Local<Derived, Interface>::isStarted() const
    {
        return _started;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    const link::Id& Local<Derived, Interface>::linkId() const
    {
        return _linkId;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    const local::Entries& Local<Derived, Interface>::entriesOut() const
    {
        return _entriesOut;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void Local<Derived, Interface>::declareImpl(auto&& entry)
    {
        if(!this->derived().addressAllowed(entry._address))
        {
            return;
        }

        auto p = _entriesOut.emplace(std::forward<decltype(entry)>(entry));
        if(_started && p.second)
        {
            this->derived().declared(*p.first);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void Local<Derived, Interface>::undeclareImpl(auto&& entry)
    {
        bool erased = !!_entriesOut.erase(entry);
        if(_started && erased)
        {
            this->derived().undeclared(entry);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    Derived& Local<Derived, Interface>::derived()
    {
        return *static_cast<Derived*>(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void Local<Derived, Interface>::started()
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void Local<Derived, Interface>::stopped()
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool Local<Derived, Interface>::addressAllowed(const transport::Address& a)
    {
        (void)a;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void Local<Derived, Interface>::declared(const local::Entry& entry)
    {
        (void)entry;
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    void Local<Derived, Interface>::undeclared(const local::Entry& entry)
    {
        (void)entry;
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    bool Local<Derived, Interface>::emitEntry(const local::Entry& entry, bool force)
    {
        if(_linkId == entry._id)
        {
            return false;
        }

        if(force)
        {
            _entriesIn.emplace(entry);
            _ras->fireDiscovered(entry._id, entry._address);
            return true;
        }
        else
        {
            bool isOut = !!_entriesOut.count(entry);

            if(!isOut)
            {
                bool isNew = _entriesIn.emplace(entry).second;
                if(isNew)
                {
                    _ras->fireDiscovered(entry._id, entry._address);
                    return true;
                }
            }
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Derived, class Interface>
    size_t Local<Derived, Interface>::emitEntries(const local::Entries& entries, bool force)
    {
        size_t res = 0;

        for(const local::Entry& entry : entries)
        {
            res += emitEntry(entry, force) ? 1 : 0;
        }

        return res;
    }
}
