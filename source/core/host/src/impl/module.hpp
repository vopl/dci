// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host/module/manifest.hpp>
#include <dci/host/module/entry.hpp>
#include <dci/cmt.hpp>
#include <memory>
#include <filesystem>

namespace dci::host::impl
{
    class Manager;

    class Module
    {
    public:
        static const module::Manifest& manifest(const std::string& mainBinaryPath);

    public:
        Module(Manager* manager, const std::filesystem::path& manifestFile);
        ~Module();

        const std::filesystem::path& manifestFile() const;
        const module::Manifest& manifest() const;

        bool attach();
        bool detach();

        bool load();
        bool unload();

        bool start();
        cmt::Future<> stopRequest();
        bool stop();

        cmt::Future<idl::Interface> createService(idl::ILid ilid);

    private:
        Manager *                   _manager;
        std::filesystem::path       _manifestFile;
        module::Manifest            _manifest;
        module::Entry *             _entry = nullptr;

        enum class State
        {
            null,

            attached,
            attachError,

            loading,
            loaded,
            loadError,
            unloading,

            starting,
            started,
            startError,
            stopping,
        } _state = State::null;

    };

    using ModulePtr = std::shared_ptr<Module>;
}
