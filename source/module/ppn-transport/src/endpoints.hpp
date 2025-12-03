// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "endpoint.hpp"

namespace dci::module::ppn::transport
{
    template <class Iface>
    class Endpoints
        : private List<std::unique_ptr<Endpoint<Iface>>>
    {
    public:
        using LowerLayer = Endpoint<Iface>;
        using LowerLayerPtr = std::unique_ptr<LowerLayer>;
        using LowerLayers = List<LowerLayerPtr>;

        LowerLayer& add(Iface&& instance)
        {
            const LowerLayerPtr& llp = this->emplace_back(std::make_unique<LowerLayer>());
            llp->subscribe(std::move(instance));
            return *llp;
        }

        void del(const Iface& instance)
        {
            for(std::size_t i(0); i<this->size(); ++i)
            {
                if((*this)[i]->_instance == instance)
                {
                    (*this)[i]->unsubscribe();
                    this->erase(this->begin() + static_cast<std::ptrdiff_t>(i));
                    break;
                }
            }
        }

        using LowerLayers::empty;
        using LowerLayers::begin;
        using LowerLayers::cbegin;
        using LowerLayers::end;
        using LowerLayers::cend;
        using LowerLayers::clear;
    private:
    };
}
