// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "../pch.hpp"

namespace dci::module::stiac::localEdge
{
    class LocalLinks
    {
    public:
        LocalLinks();
        ~LocalLinks();

        link::LocalId emplace(link::BasePtr&& link);
        link::Base* get(link::LocalId id);
        bool remove(link::LocalId id);

        bool beginRemove(link::LocalId id);
        bool endRemove(link::LocalId id);

        void deinitialize();

    private:
        std::deque<link::BasePtr>   _links;
        std::set<uint64>            _freeList;
        std::set<uint64>            _zombieList;
    };
}
