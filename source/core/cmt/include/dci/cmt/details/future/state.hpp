// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/sbs/box.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/cmt/event.hpp>
#include <dci/cmt/future/exception.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::cmt::details::future
{
    struct VoidValueStub {};

    template <class T>
    class State
        : public dci::mm::heap::Allocable<State<T>>
    {
    public:
        static constexpr bool _isVoid = std::is_void_v<T>;

        using Value = std::conditional_t
        <
            _isVoid,
            VoidValueStub,
            std::decay_t<T>
        >;

    public:
        State();
        ~State();

        bool resolved() const;
        bool resolvedValue() const;
        bool resolvedException() const;
        bool resolvedCancel() const;

        void resolveValue(auto&&... args);

        void resolveException(std::exception_ptr&& exception);
        void resolveException(const std::exception_ptr& exception);

        bool resolveCancel();

        bool resolveAs(State&& another);
        bool resolveAs(const State& another);

        void wait() const;
        cmt::Waitable& waitable() const;

        Value& value();
        const Value& value() const;

        std::exception_ptr& exception();
        const std::exception_ptr& exception() const;

    public:
        void pushContinuation(sbs::Subscription* continuation);
        void continuationRemoved();

        void pushCancelContinuation(sbs::Subscription* continuation);
        void cancelContinuationRemoved();

        void incFutureCounter();
        void decFutureCounter();

        void incPromiseCounter();
        void decPromiseCounter();

        void checkCounters();

    private:
        void executeContinuations();
        void condition(auto&& f) const;

    private:
        enum class Mode
        {
            unresolved,
            cancel,
            exception,
            value,
        };

        sbs::Box        _sbsBox4Continuations       {};
        sbs::Box        _sbsBox4CancelContinuations {};
        Mode            _mode                       {Mode::unresolved};
        mutable Event   _resolveEvent;
        union
        {
            std::exception_ptr  _exception;
            Value               _value;
        };
        std::size_t _futureCounter              {0};
        std::size_t _promiseCounter             {0};
    };



    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    State<T>::State()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    State<T>::~State()
    {
        switch(_mode)
        {
        case Mode::unresolved:
            break;
        case Mode::cancel:
            break;
        case Mode::value:
            _value.~Value();
            break;
        case Mode::exception:
            _exception.~exception_ptr();
            break;
        default:
            dbgWarn("never here");
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool State<T>::resolved() const
    {
        return Mode::unresolved != _mode;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool State<T>::resolvedValue() const
    {
        return Mode::value == _mode;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool State<T>::resolvedException() const
    {
        return Mode::exception == _mode;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool State<T>::resolvedCancel() const
    {
        return Mode::cancel == _mode;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void State<T>::resolveValue(auto&&... args)
    {
        condition([&]{return !resolved();});

        if constexpr(!_isVoid)
        {
            new(&_value) Value {std::forward<decltype(args)>(args)...};
        }

        _mode = Mode::value;
        _resolveEvent.raise();
        executeContinuations();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void State<T>::resolveException(std::exception_ptr&& exception)
    {
        condition([&]{return !resolved();});

        new(&_exception) std::exception_ptr {std::move(exception)};
        _mode = State::Mode::exception;
        _resolveEvent.raise();
        executeContinuations();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void State<T>::resolveException(const std::exception_ptr& exception)
    {
        condition([&]{return !resolved();});

        new(&_exception) std::exception_ptr {exception};
        _mode = State::Mode::exception;
        _resolveEvent.raise();
        executeContinuations();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    bool State<T>::resolveCancel()
    {
        if(resolved())
        {
            return false;
        }

        _mode = State::Mode::cancel;
        _resolveEvent.raise();
        executeContinuations();
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool State<T>::resolveAs(State&& another)
    {
        if(another.resolvedValue())
        {
            if constexpr(_isVoid)
            {
                resolveValue();
            }
            else
            {
                resolveValue(std::move(another.value()));
            }
        }
        else if(another.resolvedException())
        {
            resolveException(std::move(another.exception()));
        }
        else  if(another.resolvedCancel())
        {
            resolveCancel();
        }
        else
        {
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool State<T>::resolveAs(const State& another)
    {
        if(another.resolvedValue())
        {
            if constexpr(_isVoid)
            {
                resolveValue();
            }
            else
            {
                resolveValue(another.value());
            }
        }
        else if(another.resolvedException())
        {
            resolveException(another.exception());
        }
        else  if(another.resolvedCancel())
        {
            resolveCancel();
        }
        else
        {
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::wait() const
    {
        _resolveEvent.wait();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    cmt::Waitable& State<T>::waitable() const
    {
        return _resolveEvent;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    typename State<T>::Value& State<T>::value()
    {
        switch(_mode)
        {
        case Mode::unresolved:
            throw cmt::future::Exception("future unresolved");

        case Mode::cancel:
            throw cmt::future::Exception("future canceled");

        case Mode::exception:
            std::rethrow_exception(_exception);

        case Mode::value:
            break;
        }

        return _value;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    const typename State<T>::Value& State<T>::value() const
    {
        switch(_mode)
        {
        case Mode::unresolved:
            throw cmt::future::Exception("future unresolved");

        case Mode::cancel:
            throw cmt::future::Exception("future canceled");

        case Mode::exception:
            std::rethrow_exception(_exception);

        case Mode::value:
            break;
        }

        return _value;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    std::exception_ptr& State<T>::exception()
    {
        switch(_mode)
        {
        case Mode::unresolved:
            throw cmt::future::Exception("future unresolved");

        case Mode::cancel:
            throw cmt::future::Exception("future canceled");

        case Mode::exception:
            break;

        case Mode::value:
            throw cmt::future::Exception("future resolved to value");
        }

        return _exception;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    const std::exception_ptr& State<T>::exception() const
    {
        switch(_mode)
        {
        case Mode::unresolved:
            throw cmt::future::Exception("future unresolved");

        case Mode::cancel:
            throw cmt::future::Exception("future canceled");

        case Mode::exception:
            break;

        case Mode::value:
            throw cmt::future::Exception("future resolved to value");
        }

        return _exception;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::pushContinuation(sbs::Subscription* continuation)
    {
        _sbsBox4Continuations.push(continuation);
        _futureCounter++;
        if(resolved())
        {
            executeContinuations();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::continuationRemoved()
    {
        dbgAssert(_futureCounter>0);
        _futureCounter--;
        checkCounters();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::pushCancelContinuation(sbs::Subscription* continuation)
    {
        _sbsBox4CancelContinuations.push(continuation);
        if(resolved())
        {
            executeContinuations();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::cancelContinuationRemoved()
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::incFutureCounter()
    {
        _futureCounter++;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::decFutureCounter()
    {
        dbgAssert(_futureCounter>0);
        _futureCounter--;
        checkCounters();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::incPromiseCounter()
    {
        _promiseCounter++;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::decPromiseCounter()
    {
        dbgAssert(_promiseCounter>0);
        _promiseCounter--;
        checkCounters();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::checkCounters()
    {
//        std::size_t _futureCounter              {0};
//        std::size_t _promiseCounter             {0};

        if(!_promiseCounter)
        {
            if(!_futureCounter)
            {
                //нет никого, уничтожить
                delete this;
                return;
            }
            else
            {
                //промиса нет а желающие получить значение есть, отменить
                if(!resolved())
                {
                    resolveCancel();
                }
                return;
            }
        }
        else
        {
            if(!_futureCounter)
            {
                //промис есть а желающих получить значение нет, отменить
                if(!resolved())
                {
                    resolveCancel();
                }
                return;
            }
            else
            {
                //есть и промис и читатели, оставить все как есть
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void State<T>::executeContinuations()
    {
        switch(_mode)
        {
        case Mode::unresolved:
            dbgWarn("never here");
            break;

        case Mode::cancel:
            if(_promiseCounter)
            {
                incPromiseCounter();
                _sbsBox4CancelContinuations.activate(this, sbs::Box::del);
                if(_futureCounter)
                {
                    _sbsBox4Continuations.activate(this, sbs::Box::del);
                }
                else
                {
                    _sbsBox4Continuations.removeAndDeleteAll();
                }
                decPromiseCounter();
            }
            else if(_futureCounter)
            {
                incFutureCounter();
                dbgAssert(!_promiseCounter);
                _sbsBox4CancelContinuations.removeAndDeleteAll();
                _sbsBox4Continuations.activate(this, sbs::Box::del);
                decFutureCounter();
            }
            else
            {
                dbgWarn("wtf?");
            }
            break;

        case Mode::exception:
            [[fallthrough]];
        case Mode::value:
            _sbsBox4CancelContinuations.removeAndDeleteAll();
            _sbsBox4Continuations.activate(this, sbs::Box::del);
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class T>
    void State<T>::condition(auto&& f) const
    {
        dbgAssert(f());
        if(!f())
        {
            abort();
        }
    }
}
