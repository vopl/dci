// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/api.hpp>
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>
#include "../promise.hpp"
#include "key.hpp"
#include "state.hpp"

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Owner;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_CMT Face
        : public himpl::FaceLayout<Face, impl::task::Face>
    {
    public:
        Face();
        Face(const Face&);
        Face(Face&&);
        ~Face();

        void operator=(const Face&);
        void operator=(Face&&);

    public:
        Key key() const;
        bool isCurrent() const;
        State state() const;

    public:
        void stop(bool throwSelf = false);
        bool stopRequested() const;

        void ownTo(Owner* owner);
        Owner* owner();

        template <class T> void stopOnResolvedCancel(Promise<T>& cause);
        template <class T> void stopOnResolvedCancel(Promise<T>&& cause);

        template <class T> void stopOnResolved(Future<T>& cause);
        template <class T> void stopOnResolvedValue(Future<T>& cause);
        template <class T> void stopOnResolvedException(Future<T>& cause);
        template <class T> void stopOnResolvedCancel(Future<T>& cause);

        template <class T> void stopOnResolved(Future<T>&& cause);
        template <class T> void stopOnResolvedValue(Future<T>&& cause);
        template <class T> void stopOnResolvedException(Future<T>&& cause);
        template <class T> void stopOnResolvedCancel(Future<T>&& cause);

    private:
        sbs::Owner* sbsOwner();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face API_DCI_CMT current();







    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Promise<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.canceled() += sbsOwner * [copy=*this]() mutable
            {
                copy.stop(false);
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Promise<T>&& cause)
    {
        return stopOnResolvedCancel(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolved(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolved())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedValue(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolvedValue())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedException(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolvedException())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolvedCancel())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolved(Future<T>&& cause)
    {
        return stopOnResolved(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedValue(Future<T>&& cause)
    {
        return stopOnResolvedValue(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedException(Future<T>&& cause)
    {
        return stopOnResolvedException(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Future<T>&& cause)
    {
        return stopOnResolvedCancel(cause);
    }
}
