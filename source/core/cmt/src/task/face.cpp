// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/task/face.hpp>
#include <dci/cmt/task/owner.hpp>
#include "impl/task/face.hpp"
#include "impl/task/owner.hpp"
#include "impl/scheduler.hpp"

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face()
        : himpl::FaceLayout<Face, impl::task::Face>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(const Face& from)
        : himpl::FaceLayout<Face, impl::task::Face>(himpl::face2Impl(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(Face&& from)
        : himpl::FaceLayout<Face, impl::task::Face>(himpl::face2Impl(std::move(from)))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::~Face()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::operator=(const Face& from)
    {
        return impl() = himpl::face2Impl(from);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::operator=(Face&& from)
    {
        return impl() = himpl::face2Impl(std::move(from));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Key Face::key() const
    {
        return impl().key();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Face::isCurrent() const
    {
        return impl().isCurrent();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    State Face::state() const
    {
        return impl().state();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::stop(bool throwSelf)
    {
        return impl().stop(throwSelf);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Face::stopRequested() const
    {
        return impl().stopRequested();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::ownTo(Owner* owner)
    {
        return impl().ownTo(himpl::face2Impl(owner));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner* Face::owner()
    {
        return himpl::impl2Face<Owner>(impl().owner());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner* Face::sbsOwner()
    {
        return impl().sbsOwner();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face current()
    {
        return himpl::impl2Face<Face>(impl::task::Face{impl::Scheduler::instance().currentTask()});
    }
}
