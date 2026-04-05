/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
