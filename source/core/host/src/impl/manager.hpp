// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host/test.hpp>
#include <dci/cmt.hpp>
#include <dci/sbs/signal.hpp>
#include <dci/sbs/wire.hpp>

#include "module.hpp"

namespace dci::idl::gen::host
{
    template <idl::ISide> struct Daemon;
}

namespace dci::host::impl
{
    class Manager final
    {
    public:
        static int executeTest(const std::vector<std::string>& argv, TestStage stage, host::Manager* manager);
        static const module::Manifest& moduleManifest(const std::string& mainBinaryFullPath);

    public:
        Manager();
        ~Manager();

        void run();//блокирующий
        void stop();//запрос на выход из run

        bool startModules(std::set<std::string>&& byNames, std::set<std::string>&& byServices);

        cmt::Future<int> runTest(const std::vector<std::string>& argv, TestStage stage);
        cmt::Future<> runDaemon(const std::vector<std::string>& argv);
        cmt::Future<> runDaemons(const std::vector<std::string>& argv);

        cmt::Future<idl::Interface> createService(idl::ILid ilid);
        cmt::Future<idl::Interface> createService(const std::string& alias);
        cmt::Future<idl::Interface> getDaemonService(const std::string& name);

    private:
        bool initializeModules();
        bool deinitializeModules();

        template <class Modules, class F>
        bool massModulesOperation(const Modules& modules, const std::string& name, const F& operation);

    private:
        enum class WorkState
        {
            stopped,
            starting,
            started,
            stopping,
        } _workState = WorkState::stopped;

    private:
        std::vector<ModulePtr>                  _modules;
        std::map<std::string, Module*>          _modulesByName;
        std::multimap<idl::ILid, Module*>       _serviceProviders;
        std::multimap<std::string, idl::ILid>   _serviceAliases;

    private:
        using Daemon = dci::idl::gen::host::Daemon<idl::ISide::primary>;
        using Daemons = std::multimap<std::string, Daemon>;
        Daemons _daemons;

    private:
        cmt::task::Owner _workersOwner;
    };
}
