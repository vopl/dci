// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "keyIA.hpp"
#include "statIA/recorder.hpp"

namespace dci::module::ppn::connectivity
{
    class Reest;
}

namespace dci::module::ppn::connectivity::reest
{
    class StatIA
    {
    public:
        StatIA(Reest* srv);
        ~StatIA();

    public:
        struct SessionState
        {
            sbs::Owner          _sbsOwner;
            statIA::TimePoint   _startMoment{};
            statIA::TimePoint   _lastFlushMoment{};
            bool                _connected {};
            bool                _joined {};
        };

    public:
        void discovered();
        void newSession(node::feature::CSession<> s);
        void rekeyed(node::feature::CSession<> s, const SessionState& ssFrom);
        void regularFlush();
        real64 rating() const;
        bool dead() const;

    private:
        static real64 flushOnline(SessionState& ss, statIA::TimePoint m);
        void updateResult();

    private:
        Reest* _srv;
        real64 _rating {};

    private:
        static constexpr real64 _costDiscover       {1.0};
        static constexpr real64 _costConnectionFail {-10.0};
        static constexpr real64 _costJoinFail       {-20.0};
        static constexpr real64 _costFail           {-10.0};
        static constexpr real64 _costOffline        {-0.01};

        statIA::Recorder _recorder;

        using Sessions = std::map<node::feature::CSession<>, SessionState>;
        Sessions _sessions;
    };
}
