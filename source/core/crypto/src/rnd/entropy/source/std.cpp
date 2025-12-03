// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "std.hpp"
#include "../../instance.hpp"

namespace dci::crypto::rnd::entropy::source
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Std::available()
    {
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string_view Std::name()
    {
        return std::string_view("std::random_device{}");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Std::Std(Instance* instance)
        : Source{instance}
        , _rd{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Std::~Std()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Std::flush()
    {
        std::random_device::result_type buf[32 / sizeof(std::random_device::result_type)];

        for(std::random_device::result_type& part : buf)
            part = _rd();

        _instance->addEntropy(buf, sizeof(buf), _rd.entropy() ? sizeof(buf) : 0);
    }
}
