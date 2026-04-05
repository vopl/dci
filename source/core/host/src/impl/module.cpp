/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "module.hpp"
#include "manager.hpp"
#include <dci/host/module/manifest.hpp>
#include <dci/host/module/entry.hpp>
#include <dci/host/manager.hpp>
#include <dci/logger.hpp>
#include "../dll.hpp"

namespace dci::host::impl
{
    namespace fs = std::filesystem;

    namespace
    {
        const module::Manifest badModuleManifest;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const module::Manifest& Module::manifest(const std::string& mainBinaryPath)
    {
        boost::dll::shared_library* sl;
        try
        {
            sl = &dll(mainBinaryPath);
        }
        catch(const std::runtime_error& e)
        {
            LOGE("loading module "<<mainBinaryPath<<": " << e.what());
            return badModuleManifest;
        }

        module::Entry* entry{};
        try
        {
            entry = sl->get<module::Entry*>("dciModuleEntry");
        }
        catch(const std::runtime_error& e)
        {
            LOGE("loading module "<<mainBinaryPath<<": entry point is absent, " << e.what());
            return badModuleManifest;
        }

        return entry->manifest();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Module::Module(Manager* manager, const std::filesystem::path& manifestFile)
        : _manager(manager)
        , _manifestFile(manifestFile)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Module::~Module()
    {
        dbgAssert(State::null == _state || State::attachError == _state);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::filesystem::path& Module::manifestFile() const
    {
        return _manifestFile;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const module::Manifest& Module::manifest() const
    {
        return _manifest;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Module::attach()
    {
        switch(_state)
        {
        case State::null:           break;
        case State::attached:       return true;
        case State::attachError:    break;
        case State::loading:        return true;
        case State::loaded:         return true;
        case State::loadError:      return true;
        case State::unloading:      return true;
        case State::starting:       return true;
        case State::started:        return true;
        case State::startError:     return true;
        case State::stopping:       return true;
        }

        if(!_manifest.fromConfFile(_manifestFile.string()))
        {
            _state = State::attachError;
            LOGE("unable to load module manifest");
            return false;
        }

        _state = State::attached;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Module::detach()
    {
        unload();

        switch(_state)
        {
        case State::null:           return true;
        case State::attached:       break;
        case State::attachError:    break;
        case State::loading:        return false;
        case State::loaded:         return false;
        case State::loadError:      return false;
        case State::unloading:      return false;
        case State::starting:       return false;
        case State::started:        return false;
        case State::startError:     return false;
        case State::stopping:       return false;
        }

        _manifest.reset();
        _state = State::null;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Module::load()
    {
        attach();

        switch(_state)
        {
        case State::null:           return false;
        case State::attached:       break;
        case State::attachError:    return false;
        case State::loading:        return false;
        case State::loaded:         return true;
        case State::loadError:      return false;
        case State::unloading:      return false;
        case State::starting:       return true;
        case State::started:        return true;
        case State::startError:     return true;
        case State::stopping:       return true;
        }

        _state = State::loading;

        fs::path mainBinaryPath = _manifestFile.parent_path()/_manifest._mainBinary;

        boost::dll::shared_library* sl;
        try
        {
            sl = &dll(mainBinaryPath.string());
        }
        catch(const std::runtime_error& e)
        {
            LOGE("loading module \""<<_manifest._name<<"\" binary: "<<e.what());
            _state = State::loadError;
            return false;
        }

        dbgAssert(!_entry);

        try
        {
            _entry = sl->get<module::Entry*>("dciModuleEntry");
        }
        catch(const std::runtime_error& e)
        {
            LOGE("loading module "<<mainBinaryPath<<": entry point is absent, " << e.what());
            _state = State::loadError;
            return false;
        }

        if(!_entry->load())
        {
            LOGE("loading module \""<<_manifest._name<<"\": fail");

            _entry  = nullptr;

            sl->unload();

            _state = State::loadError;
            return false;
        }

        _state = State::loaded;

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Module::unload()
    {
        stop();

        switch(_state)
        {
        case State::null:           return true;
        case State::attached:       return true;
        case State::attachError:    return true;
        case State::loading:        return false;
        case State::loaded:         break;
        case State::loadError:      break;
        case State::unloading:      return false;
        case State::starting:       return false;
        case State::started:        return false;
        case State::startError:     return false;
        case State::stopping:       return false;
        }

        _state = State::unloading;

        if(_entry)
        {
            if(!_entry->unload())
            {
                LOGE("unloading module \""<<_manifest._name<<"\": fail");
                //ignore error
            }
        }

        _entry  = nullptr;
        _state = State::attached;

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Module::start()
    {
        load();

        switch(_state)
        {
        case State::null:           return false;
        case State::attached:       return false;
        case State::attachError:    return false;
        case State::loading:        return false;
        case State::loaded:         break;
        case State::loadError:      return false;
        case State::unloading:      return false;
        case State::starting:       return false;
        case State::started:        return true;
        case State::startError:     return false;
        case State::stopping:       return false;
        }

        _state = State::starting;

        dbgAssert(_entry);

        if(!_entry->start(himpl::impl2Face<host::Manager>(_manager)))
        {
            LOGE("starting module \""<<_manifest._name<<"\": fail");
            _state = State::startError;
            return false;
        }

        _state = State::started;

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Module::stopRequest()
    {
        switch(_state)
        {
        case State::started:
        case State::startError:
            break;

        default:
            LOGW("unable to request stop for module: wrong state");
            return cmt::readyFuture();
        }

        _state = State::stopping;

        dbgAssert(_entry);

        return _entry->stopRequest();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Module::stop()
    {
        switch(_state)
        {
        case State::null:           return true;
        case State::attached:       return true;
        case State::attachError:    return true;
        case State::loading:        return true;
        case State::loaded:         return true;
        case State::loadError:      return true;
        case State::unloading:      return true;
        case State::starting:       return false;
        case State::started:        break;
        case State::startError:     break;
        case State::stopping:       return false;
        }

        _state = State::stopping;

        dbgAssert(_entry);

        if(!_entry->stop())
        {
            LOGW("stopping module \""<<_manifest._name<<"\": fail");
            //ignore error
        }

        _state = State::loaded;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Module::createService(idl::ILid ilid)
    {
        if(State::attached == _state)
        {
            if(!start())
            {
                return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::UnableToCreateService("unable to start module")));
            }
        }

        if(State::started != _state)
        {
            return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::UnableToCreateService("module not started")));
        }

        return _entry->createService(ilid);
    }
}
