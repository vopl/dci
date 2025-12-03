// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "kernel.hpp"
#include <climits>

namespace dci::module::ppn::topology::lis::grid
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Kernel::Kernel()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Kernel::Kernel(uint8 bits, uint16 size)
        : _bits{std::clamp(bits, _bitsMin, _bitsMax)}
        , _size{std::clamp(size, _sizeMin, _sizeMax)}
        , _stepMult{std::pow(0.5, static_cast<real64>(_bits)/_size)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Kernel::Kernel(const Kernel& another)
        : _bits{another._bits}
        , _size{another._size}
        , _stepMult{another._stepMult}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Kernel::~Kernel()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Kernel::change(uint8 bits, uint16 size)
    {
        bits = std::clamp(bits, _bitsMin, _bitsMax);
        size = std::clamp(size, _sizeMin, _sizeMax);

        if(_bits == bits && _size == size)
        {
            return false;
        }

        _bits = bits;
        _size = size;
        _stepMult = std::pow(0.5, static_cast<real64>(_bits)/_size);

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Kernel& Kernel::operator=(const Kernel& another)
    {
        _bits = another._bits;
        _size = another._size;
        _stepMult = another._stepMult;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Kernel::operator==(const Kernel& another) const
    {
        return std::tie(_bits, _size) == std::tie(another._bits, another._size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Kernel::operator!=(const Kernel& another) const
    {
        return !operator==(another);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint8 Kernel::bits() const
    {
        return _bits;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 Kernel::size() const
    {
        return _size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Kernel::idx(space::Csid csid) const
    {
        real64 dist01 = static_cast<real64>(csid) / 0x1p64;

        dbgAssert(0 <= dist01);
        dbgAssert(1 >= dist01);

        if(dist01 <= std::numeric_limits<real64>::min())
        {
            return _size-1;
        }

        real64 r = log(dist01) / log(_stepMult);
        dbgAssert(std::isnormal(r));

        if(r >= _size-1)
        {
            return _size-1;
        }

        if(r <= 0)
        {
            return 0;
        }

        return static_cast<std::size_t>(r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    space::Csid Kernel::csid(std::size_t idx) const
    {
        dbgAssert(idx <= _size);

        if(idx >= _size)
        {
            return {};
        }

        constexpr real64 norma = []
        {
            real64 res = 1;
            for(std::size_t r{}; r<sizeof(space::SmallNum)*CHAR_BIT; ++r)
                res *= 2;
            return res;
        }();
        constexpr real64 max = static_cast<real64>(~space::SmallNum{});

        real64 dist01 = std::pow(_stepMult, idx);
        real64 dist64 = dist01 * norma + 0.5;

        space::SmallNum res;
        if(max <= dist64)
        {
            res = ~space::SmallNum{};
        }
        else
        {
            res = static_cast<space::SmallNum>(dist64);
        }

        if(res < static_cast<space::SmallNum>(dist64/2))
        {
            res = ~space::SmallNum{};
        }

        return static_cast<space::Csid>(res);
    }
}
