// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "joining/instance.hpp"

namespace dci::module::ppn::connectivity
{
    class Joining
        : public api::Joining<>::Opposite
        , public host::module::ServiceBase<Joining>
    {
    public:
        Joining();
        ~Joining();

    private:
        void satisfyDemand(const node::link::Id& id, api::demand::SatisfactionHolder<>&& sh);

    private:
        friend class joining::Instance;
        void instanceEmpty(const node::link::Id& id);
        cmt::Future<Tuple<node::link::Remote<>, transport::Address, real64>> fetchRdbRecord(const node::link::Id& id);
        cmt::Future<node::link::Remote<>> join(const node::link::Id& id, transport::Address&& a);

    private:
        bool                        _started{};
        rdb::Query<>                _rdbQuery;
        node::feature::Service<>    _nodeSrv;
        api::demand::Registry<>     _demandRegistry;

        struct CmpById
        {
            using is_transparent = void;
            bool operator()(const joining::Instance& a, const joining::Instance& b) const;
            bool operator()(const joining::Instance& a, const node::link::Id& b) const;
            bool operator()(const node::link::Id& a, const joining::Instance& b) const;
        };

        std::set<joining::Instance, CmpById>    _instances;
    };
}
