// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "registry.hpp"
#include "../demand.hpp"

namespace dci::module::ppn::connectivity::demand
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Registry::Registry(Demand* d)
        : api::Registry<>::Opposite{idl::interface::Initializer{}}
        , _d{d}
    {
        methods()->need() += _sol * [this](const node::link::Id& id, real64 weight)
        {
            return cmt::readyFuture(element(id).need(weight));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Registry::~Registry()
    {
        _sol.flush();
        _flushChangesTicker.stop();
        _makeSatisfiesTicker.stop();
        _changed.clear();
        _ban.clear();
        _byProbability.clear();
        _byId.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::start()
    {
        _makeSatisfiesTicker.start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::stop()
    {
        _flushChangesTicker.stop();
        _makeSatisfiesTicker.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::setIntensity(double v)
    {
        static constexpr int64 min = 50;
        static constexpr int64 max = int64{1000}*60*60*24;

        int64 ms;
        if(v <= 0)  ms = max;
        else        ms = std::clamp(static_cast<int64>(1000.0/v), min, max);

        _makeSatisfiesTicker.interval(std::chrono::milliseconds{ms});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Element& Registry::element(const node::link::Id& id)
    {
        auto iter = _byId.lower_bound(id);
        if(_byId.end() == iter || iter->id()!=id)
        {
            iter = _byId.emplace_hint(iter, id, this);
        }

        return *iter;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::construction(const Element* e)
    {
        _byProbability.insert(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::destruction(const Element* e)
    {
        _byProbability.delete_(e);

        if(_changed.erase(e) || _ban.erase(e))
        {
            _flushChangesTicker.start();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::changed(const Element* e)
    {
        if(_changed.insert(e).second)
        {
            _flushChangesTicker.start();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::ban(const Element* e)
    {
        if(_ban.insert(e).second)
        {
            _flushChangesTicker.start();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::flushChanges()
    {
        _byProbability.update(std::exchange(_changed, {}));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Registry::makeSatisfies()
    {
        if(!_ban.empty())
        {
            Element::Moment now = Element::Clock::now();

            for(auto iter{_ban.begin()}; iter!=_ban.end();)
            {
                if((*iter)->tryUnban(now))
                {
                    changed(*iter);
                    iter = _ban.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        const Element* e = _byProbability.sample();
        if(e)
        {
            dbgAssert(e->priority() > 0);
            methods()->satisfy(e->id(), e->satisfy());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Registry::CmpById::operator()(const Element& a, const Element& b) const
    {
        return a.id() < b.id();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Registry::CmpById::operator()(const Element& a, const node::link::Id& b) const
    {
        return a.id() < b;
    }
}
