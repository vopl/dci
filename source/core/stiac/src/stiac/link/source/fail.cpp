// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/stiac/link/source/fail.hpp>
#include "impl/fail.hpp"

namespace dci::stiac::link::source
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Fail::Fail(const char* cszDetails)
        : himpl::FaceLayout<Fail, impl::Fail>(cszDetails)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Fail::~Fail()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string& Fail::details() const
    {
        return impl().details();
    }

}

