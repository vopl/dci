// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../interface/side.hpp"
#include "hmDescriptorBase.hpp"
#include "hmDescriptor.hpp"
#include "mdDescriptorBase.hpp"
#include "mdDescriptor.hpp"
#include "useCounter.hpp"

namespace dci::idl::contract
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct alignas(void*) MdState
    {
        UseCounter _useCounterPrimary;
        UseCounter _useCounterOpposite;

        const MdDescriptorBase& _mddb;

        MdState(const MdDescriptorBase& mddb);

        template <class BC>
        BC& getWiresBundlesContainer();

        template <class B>
        B& getWiresBundle(uint32 offset);

        template <template <interface::Side> class CDst, template <interface::Side> class CSrc>
        const HmDescriptorBase* resolveHmd(const HmDescriptorBase* srcHmd) const;

        template <template <interface::Side> class CDst>
        const HmDescriptorBase* resolveHmd() const;

        const HmDescriptorBase* resolveHmd() const;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline MdState::MdState(const MdDescriptorBase& mddb)
        : _mddb(mddb)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class BC>
    BC& MdState::getWiresBundlesContainer()
    {
        char* bundlesContainerStart = static_cast<char*>(static_cast<void*>(this)) + sizeof(*this);
        return *static_cast<BC*>(static_cast<void*>(bundlesContainerStart));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class B>
    B& MdState::getWiresBundle(uint32 offset)
    {
        char* bundlesContainerStart = static_cast<char*>(static_cast<void*>(this)) + sizeof(*this);
        return *static_cast<B*>(static_cast<void*>(bundlesContainerStart + offset));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <interface::Side> class CDst, template <interface::Side> class CSrc>
    const HmDescriptorBase* MdState::resolveHmd(const HmDescriptorBase* srcHmd) const
    {
        //same
        if constexpr (std::is_same_v<CDst<interface::Side::primary>, CSrc<interface::Side::primary>>)
        {
            return srcHmd;
        }

        //destination is base of source (recursive)
        if constexpr (HmDescriptor<CSrc, CDst>::_declared)
        {
            return srcHmd->hmbd(HmDescriptor<CSrc, CDst>::_index)._hmd;
        }

        (void)srcHmd;

        //destination and source are from different hierarchies
        return _mddb._resolveHmd(MdDescriptor<CDst>::_id, mdDescriptor<CDst>._lid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <interface::Side> class CDst>
    const HmDescriptorBase* MdState::resolveHmd() const
    {
        return _mddb._resolveHmd(MdDescriptor<CDst>::_id, mdDescriptor<CDst>._lid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline const HmDescriptorBase* MdState::resolveHmd() const
    {
        return _mddb._resolveHmd(_mddb._id, _mddb._lid);
    }
}
