// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../space.hpp"

namespace dci::module::ppn::topology::lis::grid
{
    class Kernel
    {
    public:
        static constexpr uint8 _bitsDefault = 32;//это вместит примерно 4 миллиарда узлов
        static constexpr uint8 _bitsMin = 8;
        static constexpr uint8 _bitsMax = 52;

        static constexpr uint16 _sizeDefault = _bitsDefault * 16;//в kademlia выделяется 16 слотов на бит, последую их примеру
        static constexpr uint16 _sizeMin = 4;
        static constexpr uint16 _sizeMax = 4096;

    public:
        Kernel();
        Kernel(uint8 bits, uint16 size);
        Kernel(const Kernel& another);
        ~Kernel();

        bool change(uint8 bits, uint16 size);

        Kernel& operator=(const Kernel& another);

        bool operator==(const Kernel& another) const;
        bool operator!=(const Kernel& another) const;

        uint8 bits() const;
        uint16 size() const;

        std::size_t idx(space::Csid csid) const;
        space::Csid csid(std::size_t idx) const;

    private:
        uint8   _bits       {_bitsDefault};
        uint16  _size       {_sizeDefault};
        real64  _stepMult   {std::pow(0.5, static_cast<real64>(_bits)/_size)};
    };
}
