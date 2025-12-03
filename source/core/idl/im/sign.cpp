// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "sign.hpp"
#include <cstring>
#include <dci/crypto/rnd.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/b2h.hpp>

namespace dci::idl::im
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign::Sign()
        : _data{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign::Sign(const std::uint8_t (&data)[_size])
    {
        memcpy(this->data(), data, _size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign::Sign(const Sign& from)
    {
        memcpy(data(), from.data(), _size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign::Sign(Sign&& from)
    {
        memcpy(data(), from.data(), _size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign::~Sign()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign& Sign::operator=(const Sign& from)
    {
        memcpy(data(), from.data(), _size);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::uint8_t* Sign::data()
    {
        return _data;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::uint8_t* Sign::data() const
    {
        return _data;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Sign::toHex(std::size_t charStart, std::size_t chars) const
    {
        if(charStart >= _size*2)
        {
            return std::string();
        }
        else if(charStart+chars > _size*2)
        {
            chars = _size*2 - charStart;
        }

        std::string res;
        res.resize(_size*2);

        dci::utils::b2h(_data, _size, res.data());

        return res.substr(charStart, chars);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::fromHex(const std::string& txt)
    {
        if(txt.size() < _size*2)
        {
            return false;
        }

        return dci::utils::h2b(txt.data(), _size*2, _data);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sign::fromRnd()
    {
        dci::crypto::rnd::generate(_data, _size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::operator<(const Sign& with) const
    {
        return memcmp(data(), with.data(), _size) < 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::operator>(const Sign& with) const
    {
        return memcmp(data(), with.data(), _size) > 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::operator<=(const Sign& with) const
    {
        return memcmp(data(), with.data(), _size) <= 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::operator>=(const Sign& with) const
    {
        return memcmp(data(), with.data(), _size) >= 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::operator==(const Sign& with) const
    {
        return memcmp(data(), with.data(), _size) == 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sign::operator!=(const Sign& with) const
    {
        return memcmp(data(), with.data(), _size) != 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sign operator^(const Sign& a, const Sign& b)
    {
        std::uint8_t res[Sign::_size];
        const std::uint8_t* dataA = a.data();
        const std::uint8_t* dataB = b.data();

        for(std::size_t idx{}; idx<Sign::_size; ++idx)
            res[idx] = dataA[idx] ^ dataB[idx];

        return {res};
    }
}
