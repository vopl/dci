/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
