// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "mapper/lookout.hpp"
#include "mapper/performer.hpp"

namespace dci::module::ppn::transport::natt
{
    class Mapping;
    class Mapper
    {
    public:
        Mapper(dci::host::module::Entry* module);
        ~Mapper();

        template <class L>
        L* add(auto&&... args);

        api::Mapping<> alloc();

        host::Manager* hostManager();
        host::module::StopLocker hostModuleStopLocker();

        void lookoutChanged(mapper::Lookout* l);

    private:
        friend class Mapping;
        void start(Mapping* mapping);
        void stop(Mapping* mapping);

    private:
        mapper::PerformerPtr performerFor(const apit::Address& internal);//in thread

    private:
        dci::host::module::Entry* _module;

        using LookoutPtr = std::unique_ptr<mapper::Lookout>;
        std::set<LookoutPtr>                    _lookouts;
        cmt::Pulser                             _lookoutsChanged;

        std::map<Mapping *, cmt::task::Owner>   _workers;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class L>
    L* Mapper::add(auto&&... args)
    {
        std::unique_ptr<L> l = std::make_unique<L>(this, std::forward<decltype(args)>(args)...);
        L* raw = l.get();
        _lookouts.emplace(std::move(l));
        return raw;
    }
}
