// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/utils/intrusiveDlist.hpp>
#include <dci/cmt/task/key.hpp>
#include <dci/cmt/task/state.hpp>
#include <dci/sbs/owner.hpp>

namespace dci::cmt::impl::task
{
    class Body;
    class Owner;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Face final
        : public utils::IntrusiveDlistElement<Face, Body>
        , public utils::IntrusiveDlistElement<Face, Owner>
    {
    public:
        Face();
        Face(Body* body);
        Face(const Face&);
        Face(Face&&);
        ~Face();

        void operator=(const Face&);
        void operator=(Face&&);

    public:
        cmt::task::Key key() const;
        bool isCurrent() const;
        cmt::task::State state() const;

    public:
        void stop(bool throwSelf);
        bool stopRequested() const;

        void ownTo(Owner* owner);
        Owner* owner();

    public:
        sbs::Owner* sbsOwner();

    public:
        void detachBody();

    private:
        Body* _body {nullptr};
    };
}
