// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt.hpp>
#include <dci/primitives.hpp>
#include <dci/host/module/serviceBase.hpp>
#include <dci/idl.hpp>

namespace dci::host
{
    template <class Concrete>
    class DaemonBase
        : public idl::gen::host::Daemon<>::Opposite
        , public module::ServiceBase<Concrete>
    {
        DaemonBase(const DaemonBase&) = delete;
        void operator=(const DaemonBase&) = delete;

    public:
        DaemonBase();
        ~DaemonBase();

    protected:
        void failedImpl(primitives::ExceptionPtr&& e);

    protected:
        cmt::task::Owner                _tol;
        cmt::task::Owner                _toStaring;
        primitives::String              _name;
        idl::gen::host::daemon::State   _state = idl::gen::host::daemon::State::null;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Concrete>
    DaemonBase<Concrete>::DaemonBase()
        : idl::gen::host::Daemon<>::Opposite(idl::interface::Initializer())
    {
        //in state() -> daemon::State;
        methods()->state() += this->serviceSol() * [this]
        {
            return cmt::readyFuture(_state);
        };

        //out stateChanged(daemon::State);
        //out failed(exception);

        //in setName(string name) -> void;
        methods()->setName() += this->serviceSol() * [this](String&& name)
        {
            _name = std::move(name);
            return cmt::readyFuture(None{});
        };

        //in start(daemon::Params params) -> void;
        methods()->start() += this->serviceSol() * [this](idl::gen::Config&& config)
        {
            return cmt::spawnv<None>(_toStaring, [this, config=std::move(config)]() mutable
            {
                switch(_state)
                {
                case idl::gen::host::daemon::State::null:
                case idl::gen::host::daemon::State::stopped:
                    break;

                case idl::gen::host::daemon::State::starting:
                case idl::gen::host::daemon::State::started:
                case idl::gen::host::daemon::State::stopping:
                case idl::gen::host::daemon::State::failed:
                    throw idl::gen::host::daemon::Error{"unable to start, bad state"};
                }

                _state = idl::gen::host::daemon::State::starting;
                methods()->stateChanged(_state);

                try
                {
                    static_cast<Concrete*>(this)->startImpl(std::move(config));
                }
                catch(...)
                {
                    if(idl::gen::host::daemon::State::starting == _state)
                    {
                        _state = idl::gen::host::daemon::State::failed;
                        methods()->stateChanged(_state);
                    }

                    std::rethrow_exception(std::current_exception());
                }

                if(idl::gen::host::daemon::State::starting == _state)
                {
                    _state = idl::gen::host::daemon::State::started;
                    methods()->stateChanged(_state);
                }

                return None{};
            });
        };

        //in stop() -> void;
        methods()->stop() += this->serviceSol() * [this]()
        {
            return cmt::spawnv<None>(_tol, [this]() mutable
            {
                _toStaring.flush();

                _state = idl::gen::host::daemon::State::stopping;
                methods()->stateChanged(_state);

                static_cast<Concrete*>(this)->stopImpl();

                if(idl::gen::host::daemon::State::stopping == _state)
                {
                    _state = idl::gen::host::daemon::State::stopped;
                    methods()->stateChanged(_state);
                }

                return None{};
            });
        };

        //in service() -> interface;
        methods()->service() += this->serviceSol() * [this]()
        {
            if constexpr(requires(Concrete* c) {{c->serviceImpl()} -> std::convertible_to<idl::Interface>;})
            {
                return cmt::spawnv<idl::Interface>(_tol, [this]() mutable
                {
                    return static_cast<Concrete*>(this)->serviceImpl();
                });
            }
            else
            {
                (void)this;
                return cmt::readyFuture<idl::Interface>(dci::exception::buildInstance<idl::gen::host::daemon::NoService>());
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Concrete>
    DaemonBase<Concrete>::~DaemonBase()
    {
        this->serviceSol().flush();
        _toStaring.stop();
        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Concrete>
    void DaemonBase<Concrete>::failedImpl(ExceptionPtr&& e)
    {
        if(idl::gen::host::daemon::State::failed != _state)
        {
            _state = idl::gen::host::daemon::State::failed;
            methods()->stateChanged(_state);
        }

        methods()->failed(std::move(e));
    }
}
