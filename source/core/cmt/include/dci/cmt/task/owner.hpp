// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/api.hpp>
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>
#include <cstdint>

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_CMT Owner
        : public himpl::FaceLayout<Owner, impl::task::Owner>
    {
        Owner(const Owner&) = delete;
        Owner(Owner&&) = delete;
        void operator=(const Owner&) = delete;
        void operator=(Owner&&) = delete;

    public:
        Owner();
        ~Owner();

    public:
        bool stopRequested() const;
        bool empty() const;

        void flush(bool andWait = true);
        void stop(bool andWait = true);
        void wait();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    struct OwnedFunctor
    {
        Owner&  _owner;
        F&&     _f;

        OwnedFunctor(Owner& owner, F&& f)
            : _owner{owner}
            , _f{std::forward<F>(f)}
        {}

        OwnedFunctor(const OwnedFunctor&) = delete;
        OwnedFunctor(OwnedFunctor&&) = delete;

        void operator=(const OwnedFunctor&) = delete;
        void operator=(OwnedFunctor&&) = delete;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    OwnedFunctor<F> operator*(Owner& owner, F&& f)
    {
        return {owner, std::forward<F>(f)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    OwnedFunctor<F> operator*(Owner* owner, F&& f)
    {
        return {*owner, std::forward<F>(f)};
    }
}
