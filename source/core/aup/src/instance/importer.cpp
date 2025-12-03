// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "importer.hpp"
#include "../impl/catalog.hpp"
#include "../impl/catalog/serializeObject.hpp"
#include "../impl/storage.hpp"
#include <dci/aup/catalog/identify.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/logger.hpp>
#include <chrono>

namespace dci::aup::instance
{
    namespace fs = std::filesystem;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Importer::Importer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Importer::~Importer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Importer::setup(const fs::path& dir)
    {
        _dir = dir;
        LOGI("importer setted up for: "<<_dir.string());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Importer::start()
    {
        if(_started)
        {
            return;
        }
        _started = true;
        _ticker.start();
        LOGI("importer started");
        onTicker();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Importer::stop()
    {
        if(!_started)
        {
            return;
        }
        _started = false;
        _ticker.stop();
        LOGI("importer stopped");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void> Importer::emitStart()
    {
        return _emitStart.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, impl::Catalog*, impl::Storage*> Importer::emitData()
    {
        return _emitData.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void> Importer::emitFinish()
    {
        return _emitFinish.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Importer::onTicker()
    {
        if(!_started)
        {
            return;
        }

        auto now = std::chrono::steady_clock::now();

        try
        {
            for(fs::directory_entry de : fs::directory_iterator{_dir})
            {
                fs::path path = de.path();
                if(_someFound.try_emplace(path, now).second)
                {
                    LOGI("importer: found "<<path);
                }
            }
        }
        catch(...)
        {
            LOGE("importer: failed to enumerate import: "<<exception::currentToString());
        }

        constexpr std::chrono::steady_clock::duration delay = std::chrono::milliseconds{10*1000};

        std::chrono::steady_clock::duration minTail{delay*2};
        std::set<fs::path> ready;
        for(SomeFound::iterator iter{_someFound.begin()}; iter!=_someFound.end(); )
        {
            std::chrono::steady_clock::duration tail = delay - (now - iter->second);

            if(tail.count() <= 0)
            {
                ready.insert(iter->first);
                iter = _someFound.erase(iter);
            }
            else
            {
                minTail = std::min(tail, minTail);
                ++iter;
            }
        }

        if(!ready.empty())
        {
            _emitStart.in();
            utils::AtScopeExit se{[this]
            {
                _emitFinish.in();
            }};

            for(const fs::path& path : ready)
            {
                tryImport(path);
            }
        }
        else
        {
            if(minTail <= delay)
            {
                LOGI("importer: wait for "<<std::chrono::duration_cast<std::chrono::duration<double>>(minTail).count()<<" seconds");
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Importer::tryImport(const fs::path& path)
    {
        try
        {
            LOGI("importer: process "<<path.string());

            if(!fs::is_directory(path))
            {
                LOGE("importer: bad entry found");
                fs::remove_all(path);
                return false;
            }

            Storage s;
            s.reset(path.string(), false);
            std::optional<Bytes> catalogBlob = s.get("catalog");
            if(!catalogBlob)
            {
                LOGE("importer: no catalog found");
                fs::remove_all(path);
                return false;
            }

            impl::Catalog c;

            try
            {
                c.deserialize(*std::move(catalogBlob));
            }
            catch(...)
            {
                LOGE("importer: bad catalog: "<<exception::currentToString());
                fs::remove_all(path);
                return false;
            }

            _emitData.in(&c, &s);

            fs::remove_all(path);
            return true;
        }
        catch(...)
        {
            LOGE("importer: "<<exception::currentToString());
        }

        return false;
    }

}
