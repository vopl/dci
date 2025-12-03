// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "element.hpp"
#include "registry.hpp"

namespace dci::module::ppn::connectivity::demand
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Element::Element(const node::link::Id& id, Registry* r)
        : _id{id}
        , _r{r}
    {
        _r->construction(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Element::~Element()
    {
        _needs.clear();
        _satisfies.clear();
        _bans.clear();
        _r->destruction(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const node::link::Id& Element::id() const
    {
        return _id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Element::updatePriority() const
    {
        if(!_satisfies.empty() || !_bans.empty())
        {
            _priority = 0;
            return;
        }

        real64 priority{};
        for(const auto&[h, hs] : _needs)
        {
            priority += hs._weight;
        }

        dbgAssert(priority >= 0);
        _priority = priority;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real64 Element::priority() const
    {
        return _priority;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real64 Element::totalPriority() const
    {
        return _totalPriority;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Element::totalPriority(real64 v) const
    {
        _totalPriority = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Element::tryUnban(Moment now) const
    {
        while(!_bans.empty())
        {
            auto begin = _bans.begin();
            if(now < *begin)
            {
                return false;
            }
            _bans.erase(begin);
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::NeedHolder<> Element::need(real64 weight) const
    {
        dbgAssert(weight >= 0);

        api::NeedHolder<>::Opposite api{idl::interface::Initializer{}};
        NeedState& ns = _needs[api];
        ns._weight = weight;

        api.involvedChanged() += ns._sol * [this,api=api.weak()](bool v)
        {
            if(!v)
            {
                _needs.erase(api);
                _r->changed(this);
            }
        };

        api->reweight() += ns._sol * [this,api=api.weak()](real64 v)
        {
            dbgAssert(v >= 0);

            _needs[api]._weight = v;
            _r->changed(this);
        };

        _r->changed(this);
        return api.opposite();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::SatisfactionHolder<> Element::satisfy() const
    {
        api::SatisfactionHolder<>::Opposite api{idl::interface::Initializer{}};
        SatisfactionState& ss = _satisfies[api];

        api.involvedChanged() += ss._sol * [this,api=api.weak()](bool v)
        {
            if(!v)
            {
                _satisfies.erase(api);
                _r->changed(this);
            }
        };

        api->fail() += ss._sol * [this]()
        {
            _bans.insert(Clock::now() + std::chrono::minutes{5});
            _r->ban(this);
        };

        _r->changed(this);
        return api.opposite();
    }
}
