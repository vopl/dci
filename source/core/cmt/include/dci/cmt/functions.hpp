// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/mm/heap/allocable.hpp>
#include <dci/exception/toString.hpp>
#include <dci/logger.hpp>

#include "future.hpp"
#include "task/owner.hpp"
#include "task/body.hpp"
#include "task/face.hpp"
#include "task/stop.hpp"
#include "details/transformHolder.hpp"
#include "api.hpp"

#include <functional>

namespace dci::cmt
{
    API_DCI_CMT bool yield();

    API_DCI_CMT bool executeReadyFibers();

    using FiberEnumerationCallback = void(*)(task::State, void*);
    API_DCI_CMT void enumerateFibers(FiberEnumerationCallback, void*);

    API_DCI_CMT void spawn(task::Body* task);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct Spawn {};

    template <class Output>
    struct Spawnv {};

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F> constexpr bool spawnValid = details::TransformHolder<F>::_valid;
    template <class F> void     spawn(                    F&& f) requires spawnValid<F>;
    template <class F> void     spawn(task::Owner& owner, F&& f) requires spawnValid<F>;
    template <class F> void     spawn(task::Owner* owner, F&& f) requires spawnValid<F>;
                       Spawn    spawn();

    template <class F> void operator+=(Spawn&&, F&& f)                      requires spawnValid<F>;
    template <class F> void operator+=(Spawn&&, task::OwnedFunctor<F>&& of) requires spawnValid<F>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Output, class F> constexpr bool spawnvValid = details::TransformHolder
    <
        F,
        void,
        Promise,
        Output
    >::_valid;

    template<class Output = details::transformHolder::AutoTag, class F> auto            spawnv(                    F&& f) requires spawnvValid<Output, F>;
    template<class Output = details::transformHolder::AutoTag, class F> auto            spawnv(task::Owner& owner, F&& f) requires spawnvValid<Output, F>;
    template<class Output = details::transformHolder::AutoTag, class F> auto            spawnv(task::Owner* owner, F&& f) requires spawnvValid<Output, F>;
    template<class Output = details::transformHolder::AutoTag>          Spawnv<Output>  spawnv();

    template<class Output = details::transformHolder::AutoTag, class F> auto operator+=(Spawnv<Output>&&, F&& f)                      requires spawnvValid<Output, F>;
    template<class Output = details::transformHolder::AutoTag, class F> auto operator+=(Spawnv<Output>&&, task::OwnedFunctor<F>&& of) requires spawnvValid<Output, F>;
}

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class F>
    void spawn(F&& f) requires spawnValid<F>
    {
        return spawn(static_cast<task::Owner*>(nullptr), std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class F>
    void spawn(task::Owner& owner, F&& f) requires spawnValid<F>
    {
        return spawn(&owner, std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class F>
    void spawn(task::Owner* owner, F&& f) requires spawnValid<F>
    {
        if(owner && owner->stopRequested())
        {
            return;
        }

        using TransformHolder = details::TransformHolder<F>;

        class TaskInstance
            : public task::Body
            , public mm::heap::Allocable<TaskInstance>
        {
            TransformHolder _transformHolder;

        public:
            TaskInstance(task::Owner* owner, F&& f)
                : task::Body(owner, &callAndDestroyExecutor)
                , _transformHolder(std::forward<F>(f))
            {
            }

            static void callAndDestroyExecutor(task::Body* task, bool call, bool destroy) noexcept(true)
            {
                TaskInstance* instance = static_cast<TaskInstance*>(task);

                if(call)
                {
                    try
                    {
                        instance->_transformHolder.invoke();
                    }
                    catch(const task::Stop&)
                    {
                        //ok, task stopped
                    }
                    catch(...)
                    {
                        LOGF("unexpected exception from spawned task: "+exception::toString(std::current_exception()));
                        dbgWarn("unexpected exception from spawned task");
                        abort();
                    }
                }

                (void)destroy;
                dbgAssert(destroy);
                delete instance;
            }
        };

        TaskInstance* instance = new TaskInstance{owner, std::forward<F>(f)};
        spawn(instance);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Spawn spawn()
    {
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    void operator+=(Spawn&&, F&& f) requires spawnValid<F>
    {
        return spawn(std::forward<F>(f));
    }

    template <class F>
    void operator+=(Spawn&&, task::OwnedFunctor<F>&& of) requires spawnValid<F>
    {
        return spawn(of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Output, class F>
    auto spawnv(F&& f) requires spawnvValid<Output, F>
    {
        return spawnv<Output>(static_cast<task::Owner*>(nullptr), std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Output, class F>
    auto spawnv(task::Owner& owner, F&& f) requires spawnvValid<Output, F>
    {
        return spawnv<Output>(&owner, std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Output, class F>
    auto spawnv(task::Owner* owner, F&& f) requires spawnvValid<Output, F>
    {
        using TransformHolder = details::TransformHolder
        <
            F,
            void,
            Promise,
            Output
        >;

        if(owner && owner->stopRequested())
        {
            return typename TransformHolder::DestinationStream{}.future();
        }

        class TaskInstance
            : public task::Body
            , public mm::heap::Allocable<TaskInstance>
        {
            TransformHolder _transformHolder;
            typename TransformHolder::DestinationStream _destinationPromise;

        public:
            auto future()
            {
                return _destinationPromise.future();
            }

            TaskInstance(task::Owner* owner, F&& f)
                : task::Body(owner, &callAndDestroyExecutor)
                , _transformHolder(std::forward<F>(f))
            {
            }

            static void callAndDestroyExecutor(task::Body* task, bool call, bool destroy) noexcept(true)
            {
                TaskInstance* instance = static_cast<TaskInstance*>(task);

                if(call)
                {
                    try
                    {
                        if constexpr (
                                    std::is_same<
                                       decltype(instance->_transformHolder.invoke(&instance->_destinationPromise)),
                                       void
                                    >::value &&
                                    std::is_same<
                                       Promise<void>,
                                       typename TransformHolder::DestinationStream
                                    >::value)
                        {
                            instance->_transformHolder.invoke(&instance->_destinationPromise);

                            if(instance->_destinationPromise.charged() && !instance->_destinationPromise.resolved())
                            {
                                instance->_destinationPromise.resolveValue();
                            }
                        }
                        else if constexpr (
                                    std::is_same<
                                       Promise<decltype(instance->_transformHolder.invoke(&instance->_destinationPromise))>,
                                       typename TransformHolder::DestinationStream
                                    >::value)
                        {
                            auto ret = instance->_transformHolder.invoke(&instance->_destinationPromise);

                            dbgAssert(instance->_destinationPromise.charged());//wasnt moved

                            if(instance->_destinationPromise.charged() && !instance->_destinationPromise.resolved())
                            {
                                instance->_destinationPromise.resolveValue(ret);
                            }
                        }
                        else
                        {
                             instance->_transformHolder.invoke(&instance->_destinationPromise);
                        }
                    }
                    catch(const task::Stop&)
                    {
                        //ok, task stopped
                    }
                    catch(...)
                    {
                        if(!instance->_destinationPromise.charged())
                        {
                            dbgWarn("exception from spawned task, but promise was moved out");
                            abort();
                        }

                        if(instance->_destinationPromise.resolved())
                        {
                            if(instance->_destinationPromise.resolvedValue())
                            {
                                dbgWarn("exception from spawned task, but promise already resolved to value");
                                abort();
                            }
                            else if(instance->_destinationPromise.resolvedException())
                            {
                                dbgWarn("exception from spawned task, but promise already resolved to exception");
                                abort();
                            }
                            else //if(instance->_destinationPromise.resolvedCancel())
                            {
                                dbgAssert(instance->_destinationPromise.resolvedCancel());
                                //ok, ignore exception for canceled promise
                            }
                        }
                        else
                        {
                            instance->_destinationPromise.resolveException(std::current_exception());
                        }
                    }
                }

                (void)destroy;
                dbgAssert(destroy);
                delete instance;
             }
        };

        TaskInstance* instance = new TaskInstance{owner, std::forward<F>(f)};
        spawn(instance);

        return instance->future();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Output>
    Spawnv<Output> spawnv()
    {
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Output, class F>
    auto operator+=(Spawnv<Output>&&, F&& f) requires spawnvValid<Output, F>
    {
        return spawnv<Output>(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Output, class F>
    auto operator+=(Spawnv<Output>&&, task::OwnedFunctor<F>&& of) requires spawnvValid<Output, F>
    {
        return spawnv<Output>(of._owner, std::forward<F>(of._f));
    }
}
