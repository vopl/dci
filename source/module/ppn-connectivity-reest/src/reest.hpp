// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "reest/statsIA.hpp"
#include "reest/statsI.hpp"

namespace dci::module::ppn::connectivity
{
    class Reest
        : public api::Reest<>::Opposite
        , public host::module::ServiceBase<Reest>
    {
    public:
        Reest();
        ~Reest();

    public:
        void subscribeRegularFlush(reest::StatIA* stat);
        void unsubscribeRegularFlush(reest::StatIA* stat);

        void wantOnceFlush(reest::StatI* stat);

        void rekeyed(node::feature::CSession<> s, const reest::StatIA::SessionState& ss, const node::link::Id& id, const transport::Address& a);
        void statChanged(reest::StatIA* stat);
        void statChanged(reest::StatI* stat);

    private:
        void regularFlush();
        void onceFlush();

        void setIntensity(double v);

    private:
        bool _started {};

    private:
        reest::StatsIA _statsIA;

        dci::poll::Timer        _regularFlushTicker {std::chrono::seconds(60), true, [this]{regularFlush();}};
        std::set<reest::StatIA*>_regularFlushStats;

        dci::poll::Timer        _onceFlushTicker {std::chrono::milliseconds(50), false, [this]{onceFlush();}};
        std::set<reest::StatI*> _onceFlushStats;

    private:
        reest::StatsI _statsI;
    };
}
