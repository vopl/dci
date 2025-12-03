// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "face.hpp"
#include "body.hpp"

namespace dci::cmt::impl::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(Body* body)
        : _body(body)
    {
        if(_body)
        {
            _body->subscribe(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(const Face& from)
        : Face(from._body)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(Face&& from)
        : Face(from._body)
    {
        from.detachBody();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::~Face()
    {
        if(_body)
        {
            _body->unsubscribe(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::operator=(const Face& from)
    {
        if(_body == from._body)
        {
            return;
        }

        if(_body)
        {
            _body->unsubscribe(this);
        }

        _body = from._body;

        if(_body)
        {
            _body->subscribe(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::operator=(Face&& from)
    {
        if(_body == from._body)
        {
            from.detachBody();
            return;
        }

        if(_body)
        {
            _body->unsubscribe(this);
        }

        _body = from._body;

        if(_body)
        {
            _body->subscribe(this);
        }

        from.detachBody();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::task::Key Face::key() const
    {
        if(_body)
        {
            return _body->key();
        }

        return cmt::task::Key{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Face::isCurrent() const
    {
        if(_body)
        {
            return _body->isCurrent();
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::task::State Face::state() const
    {
        if(_body)
        {
            return _body->state();
        }

        return cmt::task::State::null;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::stop(bool throwSelf)
    {
        if(_body)
        {
            _body->stop(throwSelf);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Face::stopRequested() const
    {
        if(_body)
        {
            return _body->stopRequested();
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::ownTo(Owner* owner)
    {
        if(_body)
        {
            _body->ownTo(owner);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner* Face::owner()
    {
        if(_body)
        {
            return _body->owner();
        }

        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner* Face::sbsOwner()
    {
        if(_body)
        {
            return _body->sbsOwner();
        }

        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::detachBody()
    {
        if(_body)
        {
            _body->unsubscribe(this);
            _body = nullptr;
        }
    }

}
