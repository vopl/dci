// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/host/manager.hpp>
#include "impl/manager.hpp"

#include "idl-host.hpp"

namespace dci::host
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Manager::executeTest(const std::vector<std::string>& argv, TestStage stage)
    {
        return impl::Manager::executeTest(argv, stage, nullptr);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const module::Manifest& Manager::moduleManifest(const std::string& mainBinaryFullPath)
    {
        return impl::Manager::moduleManifest(mainBinaryFullPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::Manager()
        : himpl::FaceLayout<Manager, impl::Manager>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::~Manager()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::run()
    {
        return impl().run();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::stop()
    {
        return impl().stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manager::startModules(std::set<std::string> &&modules, std::set<std::string> &&services)
    {
        return impl().startModules(std::move(modules), std::move(services));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<int> Manager::runTest(const std::vector<std::string>& argv, TestStage stage)
    {
        return impl().runTest(argv, stage);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Manager::runDaemon(const std::vector<std::string>& argv)
    {
        return impl().runDaemon(argv);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Manager::runDaemons(const std::vector<std::string>& argv)
    {
        return impl().runDaemons(argv);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(const idl::IId& iid)
    {
        return impl().createService(idl::ILid {idl::contract::lidRegistry.get(iid._cid), iid._side});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(idl::ILid ilid)
    {
        return impl().createService(ilid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(const std::string& alias)
    {
        return impl().createService(alias);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::getDaemonService(const std::string& name)
    {
        return impl().getDaemonService(name);
    }

}
