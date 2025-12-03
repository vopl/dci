// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/sbs/implMetaInfo.hpp>
#include "api.hpp"

namespace dci::sbs
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_SBS Owner
        : public himpl::FaceLayout<Owner, impl::Owner>
    {
        Owner(const Owner&) = delete;
        void operator=(const Owner&) = delete;

    public:
        Owner();
        ~Owner();

        void flush();
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
