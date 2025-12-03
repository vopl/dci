// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/barrier.hpp>
#include "impl/barrier.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Barrier::Barrier(std::size_t depth)
        : FaceLayout(depth)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Barrier::~Barrier()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Barrier::canStride() const
    {
        return impl().canStride();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Barrier::tryStride()
    {
        return impl().tryStride();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Barrier::stride()
    {
        return impl().stride();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Barrier::wait()
    {
        return impl().wait();
    }
}
