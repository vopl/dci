// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"
#include "link.hpp"

namespace dci::module::net
{
    class Host;

    class Links
        : public sbs::Owner
    {
    public:
        Links(api::Host<>::Opposite* iface);
        ~Links();

    public:
        Link* getLink(uint32 id);
        Link* allocLink(uint32 id);
        void allocatedLinkInitialized(uint32 id, Link* link);

        void delLink(uint32 id);

        void flushChanges();

    private:
        using Interfaces        = Map<uint32, api::Link<>>;
        using Implementations   = Map<uint32, std::unique_ptr<Link>>;
        using Ids               = Set<uint32>;

    private:
        api::Host<>::Opposite * _iface = nullptr;

        //work
        Interfaces          _interfaces;
        Implementations     _implementations;

        //changes
        Implementations     _added;
        Ids                 _deleted;
    };
}
