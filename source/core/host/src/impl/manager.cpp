/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "manager.hpp"

#include <dci/logger.hpp>
#include <dci/host/exception.hpp>
#include <dci/host/manager.hpp>
#include <dci/poll.hpp>
#include <dci/exception.hpp>
#include <dci/idl/contract/lidRegistry.hpp>
#include <dci/config.hpp>
#include <dci/test/entryPoint.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/fnmatch.hpp>
#include "../dll.hpp"
#include "idl-host.hpp"

#include <cstdlib>
#include <string>
#include <vector>
#include <filesystem>

#if __has_include(<dlfcn.h>)
#   include <dlfcn.h>
#endif

#include <boost/property_tree/ptree.hpp>

namespace fs = std::filesystem;

namespace dci::host
{
    extern TestStage g_testStage;
    extern Manager* g_testManager;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace
{
    bool ends_with(const std::string& str, const std::string& suffix)
    {
        if(str.size() < suffix.size())
        {
            return false;
        }

        return str.size() - suffix.size() == str.find(suffix);
    }
}

namespace dci::host::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Manager::executeTest(const std::vector<std::string>& argv, TestStage stage, host::Manager* manager)
    {
        std::string stageStr;
        switch(stage)
        {
        case TestStage::noenv:
            stageStr = "noenv";
            break;
        case TestStage::mnone:
            stageStr = "mnone";
            break;
        case TestStage::mstart:
            stageStr = "mstart";
            break;
        case TestStage::null:
        default:
            dbgWarn("bad test stage");
            abort();
        }

        for(auto& de: fs::directory_iterator("../test"))
        {
            if(!de.is_regular_file()) continue;

            std::string fname = de.path().stem().string();
            if(!ends_with(fname, "-"+stageStr)) continue;

            fname = fs::absolute(de.path()).string();

            try
            {
                dll(fname);
            }
            catch(const std::runtime_error& e)
            {
                throw exception::TestFail(std::string("unable to load test module ")+fname+", "+e.what());
            }
        }

        g_testStage = stage;
        g_testManager = manager;
        int res = dci::test::entryPoint(argv);
        g_testStage = TestStage::null;
        g_testManager = nullptr;

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const module::Manifest& Manager::moduleManifest(const std::string& mainBinaryFullPath)
    {
        return Module::manifest(mainBinaryFullPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::Manager()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::~Manager()
    {
        dbgAssert(_workersOwner.empty());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::run()
    {
        switch(_workState)
        {
        case WorkState::stopped:
            break;

        case WorkState::starting:
            throw exception::RunFail("starting already in progress");

        case WorkState::started:
            throw exception::RunFail("already started");

        case WorkState::stopping:
            throw exception::RunFail("stopping in progress");

        default:
            dbgWarn("unknown work state");
            abort();
        }

        {
            std::error_code ec = poll::initialize();
            if(ec)
            {
                throw exception::RunFail("unable to initialize poller: "+ec.message());
            }
        }

        _workState = WorkState::starting;

        if(!initializeModules())
        {
            throw exception::RunFail("modules initialization failed");
        }

        _workState = WorkState::started;

        if(std::error_code ec = poll::run())
        {
            throw exception::RunFail("unable to run poller: "+ec.message());
        }

        if(WorkState::started == _workState)
        {
            stop();
            poll::run(false);
        }

        {
            std::error_code ec = poll::deinitialize();
            if(ec)
            {
                LOGW("poll deinitialize: "<<ec);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::stop()
    {
        switch(_workState)
        {
        case WorkState::started:
            break;

        case WorkState::stopped:
        case WorkState::stopping:
            return ;

        default:
            dbgWarn("unknown work state");
            abort();
        }

        _workState = WorkState::stopping;

        cmt::spawn() += _workersOwner * [this]() mutable
        {
            {
                Daemons daemons{std::move(_daemons)};
                std::vector<cmt::Future<None>> stops;
                stops.reserve(daemons.size());
                for(auto& daemon : daemons)
                {
                    if(daemon.second)
                    {
                        stops.emplace_back(daemon.second->stop());
                    }
                }

                for(const cmt::Future<None>& stop : stops)
                {
                    stop.wait();
                }
            }

            if(!deinitializeModules())
            {
                LOGE("modules deinitialization failed");
            }

            cmt::task::current().ownTo(nullptr);
            _workersOwner.stop();

            _workState = WorkState::stopped;

            std::error_code ec = poll::stop();
            if(ec)
            {
                LOGE("stop poll: "<<ec);
            }

        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manager::startModules(std::set<std::string>&& byNames, std::set<std::string>&& byServices)
    {
        std::vector<Module*> selected;

        const auto match = [](const std::set<std::string>& patterns, const std::string& value)
        {
            for(const std::string& pattern : patterns)
            {
                if(!pattern.empty() && !value.empty() && utils::fnmatch(pattern.c_str(), value.c_str()))
                {
                    return true;
                }
            }

            return false;
        };

        for(const ModulePtr& module : _modules)
        {
            const module::Manifest& manifest = module->manifest();

            if(match(byNames, manifest._name))
            {
                selected.emplace_back(module.get());
                continue;
            }

            for(const module::Manifest::ServiceId& serviceId : manifest._serviceIds)
            {
                if(byServices.contains(serviceId._iid.toText()) || match(byServices, serviceId._alias))
                {
                    selected.emplace_back(module.get());
                    break;
                }
            }
        }

        return massModulesOperation(selected, "startModule", [](Module* m)
        {
            return m->start();
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<int> Manager::runTest(const std::vector<std::string>& argv, TestStage stage)
    {
        return cmt::spawnv() += _workersOwner * [=,this]()
        {
            return executeTest(argv, stage, himpl::impl2Face<host::Manager>(this));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Manager::runDaemon(const std::vector<std::string>& argv)
    {
        if(argv.empty())
        {
            return cmt::readyFuture<void>(std::make_exception_ptr(exception::DaemonRunFail("empty argv")));
        }

        std::string moduleName;
        std::string::size_type dotPos = argv[0].find('.');
        if(std::string::npos == dotPos)
        {
            moduleName = argv[0];
        }
        else
        {
            moduleName = argv[0].substr(0, dotPos);
        }

        auto iter = _modulesByName.find(moduleName);
        if(_modulesByName.end() == iter)
        {
            return cmt::readyFuture<void>(std::make_exception_ptr(exception::DaemonRunFail("module \""+moduleName+"\" not found")));
        }

        Module* module = iter->second;
        cmt::Future<idl::Interface> fd = module->createService(dci::idl::gen::host::Daemon<>::lid());

        return cmt::spawnv() += _workersOwner * [fd=std::move(fd), argv=std::move(argv), this]()
        {
            try
            {
                idl::gen::host::Daemon<> dmn = fd.value();

                if(!dmn)
                {
                    throw exception::DaemonRunFail("module \""+argv[0]+"\" provides null daemon instance");
                }

                _daemons.emplace(std::make_pair(argv[0], dmn));

                idl::gen::Config cfg = config::cnvt(config::parse(std::vector<std::string>{argv.begin()+1, argv.end()}));

                dmn->setName(argv[0]).value();
                dmn->start(std::move(cfg)).value();
            }
            catch(...)
            {
                std::rethrow_exception(
                    dci::exception::buildInstance<exception::DaemonRunFail>(
                                std::current_exception())
                );
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Manager::runDaemons(const std::vector<std::string>& argv)
    {
        if(argv.size() < 2)
        {
            return cmt::readyFuture<void>(std::make_exception_ptr(exception::DaemonRunFail("bad argv")));
        }

        std::size_t amount {};
        try
        {
            amount = static_cast<std::size_t>(std::stoul(argv[0]));
        }
        catch(...)
        {
            return cmt::readyFuture<void>(dci::exception::buildInstance<exception::DaemonRunFail>(std::current_exception(), "bad argv"));
        }

        std::vector<std::string> oneArgv{++argv.begin(), argv.end()};

        return cmt::spawnv() += _workersOwner * [amount, oneArgv=std::move(oneArgv), this]()
        {
            std::vector<cmt::Future<>> res;
            for(std::size_t i{0}; i<amount; ++i)
            {
                res.push_back(runDaemon(oneArgv));
            }

            for(cmt::Future<>& f : res)
            {
                f.wait();

                if(f.resolvedException())
                {
                    std::rethrow_exception(f.detachException());
                }
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<idl::ILid> Manager::resolveAlias(const std::string& alias)
    {
        if(idl::ILid ilid; ilid.fromIidText(alias))
        {
            return {ilid};
        }

        if(const auto iter = _serviceAliases.find(alias); _serviceAliases.end() != iter)
        {
            return {iter->second};
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(idl::ILid ilid)
    {
        const auto iter = _serviceProviders.find(ilid);

        if(_serviceProviders.end() == iter)
        {
            std::string descr = "iid not registred: "+ilid.toIidText();
            return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::UnableToCreateService(std::move(descr))));
        }

        Module* module = iter->second;

        return module->createService(ilid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(const std::string& alias)
    {
        std::optional<idl::ILid> ilidOpt = resolveAlias(alias);
        if(!ilidOpt)
        {
            std::string descr = "alias not registred: "+alias;
            return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::UnableToCreateService(std::move(descr))));
        }

        if(!ilidOpt.value())
        {
            std::string descr = "iid not registred: "+alias;
            return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::UnableToCreateService(std::move(descr))));
        }

        return createService(ilidOpt.value());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::getDaemonService(const std::string& name)
    {
        Daemons::iterator iter = _daemons.find(name);
        if(_daemons.end() == iter)
        {
            return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::DaemonGetFail("daemon \""+name+"\" not found")));
        }

        if(!iter->second)
        {
            return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::DaemonGetFail("daemon \""+name+"\" empty")));
        }

        return iter->second->service();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manager::initializeModules()
    {
        fs::path modulesDir = fs::current_path() / "../module";

        if(!fs::exists(modulesDir))
        {
            LOGE("modules initialization: modules directory is absent");
            return false;
        }

        try
        {
            modulesDir = fs::canonical(modulesDir);
        }
        catch(...)
        {
            LOGE(dci::exception::currentToString());
            return false;
        }

        if(!fs::exists(modulesDir))
        {
            LOGE("modules initialization: modules directory is absent");
            return false;
        }

        bool hasFails = false;

        fs::directory_iterator diter(modulesDir);
        fs::directory_iterator dend;
        for(; diter!=dend; ++diter)
        {
            const fs::path manifestPath = diter->path();

            if(".manifest" != manifestPath.extension())
                continue;

            if(!fs::is_regular_file(manifestPath))
                continue;

            ModulePtr module = std::make_unique<Module>(this, manifestPath);

            if(!module->attach())
            {
                LOGE("modules initialization: unable to attach " << manifestPath);
                hasFails = true;
                continue;
            }

            const module::Manifest& manifest = module->manifest();
            _modulesByName.emplace(manifest._name, module.get());

            for(const module::Manifest::ServiceId& serviceId : manifest._serviceIds)
            {
                idl::ILid ilid{idl::contract::lidRegistry.emplace(serviceId._iid._cid), serviceId._iid._side};
                _serviceProviders.emplace(ilid, module.get());
                if(!serviceId._alias.empty())
                {
                    _serviceAliases.emplace(serviceId._alias, ilid);
                }
            }

            _modules.emplace_back(std::move(module));
        }

        return !hasFails;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manager::deinitializeModules()
    {
        bool res = massModulesOperation(_modules, "detach", [](const ModulePtr& m)
        {
            return m->detach();
        });

        _modules.clear();
        _modulesByName.clear();
        _serviceProviders.clear();
        _serviceAliases.clear();

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Modules, class F>
    bool Manager::massModulesOperation(const Modules& modules, const std::string& name, const F& operation)
    {
        bool hasErrors = false;
        std::vector<cmt::Future<bool>> results;
        results.reserve(modules.size());

        for(const auto& m : modules)
        {
            results.emplace_back(cmt::spawnv() += _workersOwner * [&]
            {
                return operation(m);
            });
        }

        for(std::size_t i(0); i<results.size(); ++i)
        {
            cmt::Future<bool>& r = results[i];

            if(r.waitException())
            {
                LOGE(name<<" failed for module "<<modules[i]->manifestFile()<<" with exception: "<<dci::exception::toString(r.detachException()));
                hasErrors = true;
            }
            else if(!r.value())
            {
                LOGE(name<<" failed for module "<<modules[i]->manifestFile());
                hasErrors = true;
            }
        }

        return !hasErrors;
    }
}
