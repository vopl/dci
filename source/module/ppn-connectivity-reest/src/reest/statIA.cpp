// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "statIA.hpp"
#include "../reest.hpp"
#include <fstream>

namespace dci::module::ppn::connectivity::reest
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StatIA::StatIA(Reest* srv)
        : _srv{srv}
        , _recorder{}
    {
        _srv->subscribeRegularFlush(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StatIA::~StatIA()
    {
        _sessions.clear();
        _srv->unsubscribeRegularFlush(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatIA::discovered()
    {
        auto m = statIA::now();

        _recorder.fix(m, _costDiscover);
        updateResult();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatIA::newSession(node::feature::CSession<> s)
    {
        SessionState* ss = &_sessions[s];
        ss->_startMoment = statIA::now();
        ss->_lastFlushMoment = ss->_startMoment;

        //out connected();
        s->connected() += ss->_sbsOwner * [=]
        {
            ss->_connected = true;
        };

        //out idSpecified(link::Id);
        s->idSpecified() += ss->_sbsOwner * [=, this](const node::link::Id& id)
        {
            if(id != std::get<0>(stat2Key(*this)))
            {
                {
                    ss->_sbsOwner.flush();
                    Sessions::node_type ssLifeKeeper = _sessions.extract(s);
                    _srv->rekeyed(s, *ss, id, std::get<1>(stat2Key(*this)));
                }

                updateResult();
            }
        };

        //out failed(exception);
        s->failed() += ss->_sbsOwner * [=,this](const ExceptionPtr&)
        {
            auto m = statIA::now();

            if(!ss->_connected)
            {
                _recorder.fix(m, _costConnectionFail);
            }
            else if(!ss->_joined)
            {
                _recorder.fix(m, _costJoinFail);
            }
            else
            {
                _recorder.fix(m, _costFail);
            }
            updateResult();
        };

        //out joined();
        s->joined() += ss->_sbsOwner * [ss,this](const node::link::Remote<>&)
        {
            auto m = statIA::now();

            ss->_joined = true;
            _recorder.dropNegatives();
            _recorder.fix(m, flushOnline(*ss, m));
            updateResult();
        };

        //out closed();
        s->closed() += ss->_sbsOwner * [ss,s=s.weak(),this]
        {
            auto m = statIA::now();

            if(ss->_joined)
            {
                _recorder.fix(m, flushOnline(*ss, m));
                ss->_joined = false;
            }
            _sessions.erase(s);
            updateResult();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatIA::rekeyed(node::feature::CSession<> s, const SessionState& ssFrom)
    {
        SessionState* ss = &_sessions[s];

        ss->_startMoment = ssFrom._startMoment;
        ss->_lastFlushMoment = ssFrom._lastFlushMoment;
        ss->_connected = true; dbgAssert(ssFrom._connected);
        ss->_joined = false; dbgAssert(!ssFrom._joined);

        //out failed(exception);
        s->failed() += ss->_sbsOwner * [ss,this](const ExceptionPtr&)
        {
            auto m = statIA::now();

            if(!ss->_joined)
            {
                _recorder.fix(m, _costJoinFail);
            }
            else
            {
                _recorder.fix(m, _costFail);
            }
            updateResult();
        };

        //out joined();
        s->joined() += ss->_sbsOwner * [ss,this](const node::link::Remote<>&)
        {
            auto m = statIA::now();

            ss->_joined = true;
            _recorder.dropNegatives();
            _recorder.fix(m, flushOnline(*ss, m));
            updateResult();
        };

        //out closed();
        s->closed() += ss->_sbsOwner * [ss,s=s.weak(),this]
        {
            auto m = statIA::now();

            if(ss->_joined)
            {
                _recorder.fix(m, flushOnline(*ss, m));
                ss->_joined = false;
            }
            _sessions.erase(s);
            updateResult();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatIA::regularFlush()
    {
        updateResult();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real64 StatIA::rating() const
    {
        return _rating;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool StatIA::dead() const
    {
        if(!_sessions.empty())
        {
            return false;
        }

        for(std::size_t i{0}; i<_recorder.counters().size(); ++i)
        {
            if(_recorder.counters()[i] > 0)
            {
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real64 StatIA::flushOnline(SessionState& ss, statIA::TimePoint m)
    {
        real64 amount = statIA::toSeconds(m - ss._lastFlushMoment);
        ss._lastFlushMoment = m;

        return amount;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatIA::updateResult()
    {
        std::size_t onlinesCount{};
        {
            auto m = statIA::now();

            real64 onlineAmount{};
            for(auto&[s, ss] : _sessions)
            {
                if(ss._joined)
                {
                    onlineAmount += flushOnline(ss, m);
                    ++onlinesCount;
                }
            }

            if(onlineAmount > 0)
            {
                _recorder.fix(m, onlineAmount);
            }
            else
            {
                _recorder.fix(m, _costOffline * _recorder.unfixedVolume(m));
            }
        }

        real64 rating = 0;
        {
            real64 weight = 1.0;
            real64 weightSum = 0.0;
            for(statIA::Recorder::Counter rc : _recorder.counters())
            {
                rating += weight * static_cast<real64>(rc);
                weightSum += weight;
                weight /= 2;
            }
            rating /= weightSum;

            if(onlinesCount)
            {
                rating = std::clamp(rating, std::numeric_limits<real64>::min(), 1.0);
            }
            else
            {
                rating = std::clamp(rating, -1.0, 1.0);
            }
        }

//        {
//            auto id = std::get<0>(stat2Key(*this));
//            auto addr = std::get<1>(stat2Key(*this));
//            std::ofstream out{utils::b2h(id).c_str(), std::ios_base::app | std::ios_base::out};
//            out<<"addr: "<<addr.value<<", rating: "<<rating<<std::endl;
//        }

        if(rating != _rating)
        {
            _rating = rating;
            _srv->statChanged(this);
        }
    }

}
