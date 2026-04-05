/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "promiseNullInitializer.hpp"
#include "details/future/statePtr.hpp"
#include "details/transformHolder.hpp"
#include <dci/utils/dbg.hpp>

#include <memory>
#include <type_traits>

namespace dci::cmt
{
    template<class T>
    class Future;

    template<class T>
    struct PromiseCanceled;

    template<class T = void>
    class Promise
    {
    public:
        using StatePtr = details::future::StatePtr<T, false>;
        using State = details::future::State<T>;
        static constexpr bool _isVoid = State::_isVoid;
        using Value = typename State::Value;

    public:
        using Future = cmt::Future<T>;
        using Opposite = Future;

        explicit Promise(PromiseNullInitializer);
        Promise();
        Promise(const Promise& other);
        Promise(Promise&& other);

        Promise& operator=(const Promise& other);
        Promise& operator=(Promise&& other);

        ~Promise();

        bool charged() const;
        void uncharge();

        //////////////////////////////////////////////////
        cmt::Waitable& waitable();
        void wait() const;
        bool waitValue() const;
        bool waitException() const;
        bool waitCancel() const;

        //////////////////////////////////////////////////
        Future future();

        //////////////////////////////////////////////////
        bool resolved() const;
        bool resolvedValue() const;
        bool resolvedException() const;
        bool resolvedCancel() const;

        void resolveValue(auto&&... args);

        void resolveException(std::exception_ptr&& exception);
        void resolveException(const std::exception_ptr& exception);

        bool resolveCancel();

        bool resolveAs(Future&& another);
        bool resolveAs(const Future& another);

        void continueAs(Future&& another);
        void continueAs(const Future& another);

    public:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class F>
        static constexpr bool canceledValid =
            details::TransformHolder<F>::_valid &&
            std::is_same_v<typename details::TransformHolder<F>::TransformResult, void>;

        template <class F> void canceled(                   F&&) requires canceledValid<F>;
        template <class F> void canceled(sbs::Owner& owner, F&&) requires canceledValid<F>;
        template <class F> void canceled(sbs::Owner* owner, F&&) requires canceledValid<F>;

        PromiseCanceled<T> canceled();

    private:
        void condition(auto&& f) const;

    private:
        StatePtr _statePtr;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    struct PromiseCanceled
    {
        Promise<T>& _promise;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>::Promise(PromiseNullInitializer)
        : _statePtr()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>::Promise()
        : _statePtr(new State)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>::Promise(const Promise& other)
        : _statePtr(other._statePtr)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>::Promise(Promise&& other)
        : _statePtr(std::move(other._statePtr))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>& Promise<T>::operator=(const Promise& other)
    {
        _statePtr = other._statePtr;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>& Promise<T>::operator=(Promise&& other)
    {
        _statePtr = std::move(other._statePtr);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    Promise<T>::~Promise()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::charged() const
    {
        return _statePtr.charged();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::uncharge()
    {
        _statePtr.uncharge();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    cmt::Waitable& Promise<T>::waitable()
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->waitable();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::wait() const
    {
        condition([&]{return _statePtr.charged();});
        if(!_statePtr->resolved())
        {
            _statePtr->wait();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::waitValue() const
    {
        wait();
        return resolvedValue();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::waitException() const
    {
        wait();
        return resolvedException();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::waitCancel() const
    {
        wait();
        return resolvedCancel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    typename Promise<T>::Future Promise<T>::future()
    {
        return Future(_statePtr.raw());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolved() const
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolved();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolvedValue() const
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolvedValue();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolvedException() const
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolvedException();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolvedCancel() const
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolvedCancel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::resolveValue(auto&&... args)
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolveValue(std::forward<decltype(args)>(args)...);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::resolveException(std::exception_ptr&& exception)
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolveException(std::move(exception));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::resolveException(const std::exception_ptr& exception)
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolveException(exception);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolveCancel()
    {
        condition([&]{return _statePtr.charged();});
        return _statePtr->resolveCancel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolveAs(Future&& another)
    {
        return _statePtr->resolveAs(std::move(*another._statePtr.raw()));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool Promise<T>::resolveAs(const Future& another)
    {
        return _statePtr->resolveAs(*another._statePtr.raw());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::continueAs(Future&& another)
    {
        if(resolved())
        {
            //self resolved, no more actions required
            another.resolveCancel();
            return;
        }

        if(another.resolved())
        {
            //another resolved, try to resolve self
            resolveAs(std::move(another));
            return;
        }

        std::shared_ptr<sbs::Owner> ownerPtr = std::make_shared<sbs::Owner>();

        Future another2 = another;
        another2.then() += ownerPtr.get() * [ownerPtr, self=*this, another=std::move(another)]() mutable
        {
            //another resolved, try to resolve self
            ownerPtr->flush();
            self.resolveAs(std::move(another));
        };

        future().then() += ownerPtr.get() * [ownerPtr, another=std::move(another2)]() mutable
        {
            //self resolved, no more actions required
            ownerPtr->flush();
            another.resolveCancel();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::continueAs(const Future& another)
    {
        if(resolved())
        {
            //self resolved, no more actions required
            another.resolveCancel();
            return;
        }

        if(another.resolved())
        {
            //another resolved, try to resolve self
            resolveAs(another);
            return;
        }

        std::shared_ptr<sbs::Owner> ownerPtr = std::make_shared<sbs::Owner>();

        another.then() += ownerPtr.get() * [ownerPtr, self=*this, another]() mutable
        {
            //another resolved, try to resolve self
            ownerPtr->flush();
            self.resolveAs(another);
        };

        future().then() += ownerPtr.get() * [ownerPtr, another]() mutable
        {
            //self resolved, no more actions required
            ownerPtr->flush();
            another.resolveCancel();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    template <class F>
    void Promise<T>::canceled(F&& f) requires canceledValid<F>
    {
        return canceled(static_cast<sbs::Owner*>(nullptr), std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    template <class F>
    void Promise<T>::canceled(sbs::Owner& owner, F&& f) requires canceledValid<F>
    {
        return canceled(&owner, std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    template <class F>
    void Promise<T>::canceled(sbs::Owner* owner, F&& f) requires canceledValid<F>
    {
        condition([&]{return _statePtr.charged();});

        using TransformHolder = details::TransformHolder<F>;

        class Continuation
            : public sbs::Subscription
#if !(__GNUC__ == 14) //https://gcc.gnu.org/bugzilla/show_bug.cgi?id=115232
            , public mm::heap::Allocable<Continuation>
#endif
        {
            TransformHolder _transformHolder;

        public:
            Continuation(sbs::Owner* owner, F&& f)
                : sbs::Subscription(&Continuation::activator, owner)
                , _transformHolder(std::forward<F>(f))
            {
            }

            static void activator(Subscription* cp, void* sp, std::uint_fast8_t flags)
            {
                dbgAssert(cp);
                Continuation* self = static_cast<Continuation*>(cp);

                if(flags & sbs::Subscription::act)
                {
                    try
                    {
                        self->_transformHolder.invoke();
                    }
                    catch(...)
                    {
                        dbgWarn("exception from promise-continuation, but no results are expected");
                        abort();
                    }
                }

                if(flags & sbs::Subscription::del)
                {
                    delete self;

                    if(sp)
                    {
                        State* rawState = static_cast<State*>(sp);
                        rawState->cancelContinuationRemoved();
                    }
                }
            }
        };

        _statePtr->pushCancelContinuation(new Continuation{owner, std::forward<F>(f)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    PromiseCanceled<T> Promise<T>::canceled()
    {
        return {*this};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void Promise<T>::condition(auto&& f) const
    {
        dbgAssert(f());
        if(!f())
        {
            abort();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T, class F>
    void operator+=(PromiseCanceled<T>&& canceled, F&& f) requires Promise<T>::template canceledValid<F>
    {
        return canceled._promise.canceled(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T, class F>
    void operator+=(PromiseCanceled<T>& canceled, F&& f) requires Promise<T>::template canceledValid<F>
    {
        return canceled._promise.canceled(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T, class F>
    void operator+=(PromiseCanceled<T>&& canceled, sbs::OwnedFunctor<F>&& of) requires Promise<T>::template canceledValid<F>
    {
        return canceled._promise.canceled(of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T, class F>
    void operator+=(PromiseCanceled<T>& canceled, sbs::OwnedFunctor<F>&& of) requires Promise<T>::template canceledValid<F>
    {
        return canceled._promise.canceled(of._owner, std::forward<F>(of._f));
    }
}

#include <dci/cmt/future.hpp>
