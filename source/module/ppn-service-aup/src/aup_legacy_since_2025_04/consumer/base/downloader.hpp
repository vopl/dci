// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    class Base;
}

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    class Remote;
    namespace remote
    {
        using Ptr = std::unique_ptr<Remote>;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Downloader
    {
    public:
        Downloader(Base* b, const Oid& oid, int priority);
        ~Downloader();

        void involve(Remote* r);
        void uninvolve(Remote* r);

        const Oid& oid() const;
        int priority() const;
        void canWork();

    private:
        void worker();

    private:
        Base *  _b {};
        Oid     _oid {};
        int     _priority {};

        cmt::task::Owner    _taskOwner;

        std::set<Remote*>   _candidates;
        bool                _canWork {};

        cmt::Notifier       _awaker;

    private:
        std::set<Remote*> _remotesPresent;
        std::set<Remote*> _remotesMissingAndWanted;
        std::set<Remote*> _remotesMissingAndUnwanted;

    };
}
