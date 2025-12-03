// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../pch.hpp"
#include "linkIdOperations.hpp"

namespace dci::module::stiac::localEdge
{
    class RemoteLinks
    {
    public:
        RemoteLinks();
        ~RemoteLinks();

        bool emplace(link::BasePtr&& link, link::RemoteId id);
        link::Base* get(link::RemoteId id);
        bool remove(link::RemoteId id);

        bool beginRemove(link::RemoteId id);
        bool endRemove(link::RemoteId id);

        void deinitialize();

    private:
        std::deque<link::BasePtr>   _links;
        std::set<uint64>            _zombieList;
    };
}
