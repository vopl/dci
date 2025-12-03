// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "storage.hpp"
#include <dci/aup/exception.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/logger.hpp>

namespace dci::aup::impl
{
    namespace fs = std::filesystem;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::Storage()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::~Storage()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::reset()
    {
        _place.clear();
        _autoFixIfCan = true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::reset(const std::string &place, bool autoFixIfCan)
    {
        if(place.empty())
        {
            reset();
            return;
        }

        _place = fs::weakly_canonical(place);
        _autoFixIfCan = autoFixIfCan;

        if(_autoFixIfCan)
        {
            fs::create_directories(_place);
        }
    }

    namespace
    {
        void enumerateContent(const fs::path& root, bool autoFixIfCan, const auto& f)
        {
            for(const fs::directory_entry& de : fs::recursive_directory_iterator{root})
            {
                if(!de.is_regular_file())
                {
                    continue;
                }

                std::string oidTxt;
                for(const auto& part : de.path().lexically_relative(root))
                {
                    oidTxt += part.string();
                }

                if("catalog" == oidTxt)
                {
                    continue;
                }

                Oid oid;
                if(oidTxt.size() != oid.size()*2)
                {
                    if(autoFixIfCan)
                    {
                        fs::remove(de);
                    }
                    continue;
                }

                if(!utils::h2b(oidTxt.data(), oidTxt.size(), oid.data()))
                {
                    if(autoFixIfCan)
                    {
                        fs::remove(de);
                    }
                    continue;
                }

                f(de, oid);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::import(Storage* from)
    {
        enumerateContent(from->_place, false, [&](const fs::directory_entry& de, const Oid& oid)
        {
            if(has(oid))
            {
                //already
                return;
            }

            fs::path path = filePath(oid);
            if(path.empty())
            {
                //uninitialized?
                return;
            }

            try
            {
                fs::create_directories(path.parent_path());
                fs::rename(de.path(), path);
            }
            catch(...)
            {
                LOGW("unable to import storage entry: "<<de.path().string()<<" -> "<<path.string()<<": "<<exception::currentToString());
                throw;
            }

            //LOGI("imported storage entry: "<<de.path().string()<<" -> "<<path.string());
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Storage::dropOthersThan(const Set<Oid>& keep)
    {
        uint32 res{};
        enumerateContent(_place, _autoFixIfCan, [&](const fs::directory_entry& de, const Oid& oid)
        {
            if(!keep.count(oid))
            {
                fs::remove(de.path());
                res++;
            }
        });

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<Oid> Storage::enumerate()
    {
        Set<Oid> res;
        enumerateContent(_place, _autoFixIfCan, [&](const fs::directory_entry&, const Oid& oid)
        {
            res.insert(oid);
        });

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const std::string& localPath, Bytes&& blob)
    {
        return put_(filePath(localPath), std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const std::string& localPath, std::FILE* f)
    {
        return put_(filePath(localPath), f);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has(const std::string& localPath)
    {
        return has_(filePath(localPath));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get(const std::string& localPath, uint32 offset, uint32 size)
    {
        return get_(filePath(localPath), offset, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del(const std::string& localPath)
    {
        return del_(filePath(localPath));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const Oid& oid, Bytes&& blob)
    {
        return put_(filePath(oid), std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const Oid& oid, std::FILE* f)
    {
        return put_(filePath(oid), f);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has(const Oid& oid)
    {
        return has_(filePath(oid));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get(const Oid& oid, uint32 offset, uint32 size)
    {
        return get_(filePath(oid), offset, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del(const Oid& oid)
    {
        return del_(filePath(oid));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::delAll(bool andPlaceDirectory)
    {
        if(_place.empty())
        {
            return;
        }

        try
        {
            if(andPlaceDirectory)
            {
                fs::remove_all(_place);
            }
            else
            {
                for(const fs::path& p: fs::directory_iterator(_place))
                {
                    fs::remove_all(p);
                }
            }
        }
        catch(const fs::filesystem_error& e)
        {
            std::throw_with_nested(aup::Exception{"storage delAll fail: "+e.code().message()});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put_(const fs::path& path, Bytes&& blob_)
    {
        if(path.empty())
        {
            return;
        }

        try
        {
            Bytes blob {std::move(blob_)};
            fs::create_directories(path.parent_path());

            {
                std::FILE* out = fopen(path.string().c_str(), "wb");
                if(!out)
                {
                    throw std::system_error(errno, std::generic_category(), "unable to open "+path.string());
                }
                utils::AtScopeExit se{[&]{fclose(out);}};

                bytes::Cursor c {blob.begin()};
                while(!c.atEnd())
                {
                    uint32 s = c.continuousDataSize();
                    if(s != fwrite(c.continuousData(), 1, s, out))
                    {
                        throw std::system_error(errno, std::generic_category(), "unable to write "+path.string());
                    }

                    c.advanceChunks(1);
                }
            }
        }
        catch(const std::system_error& e)
        {
            std::throw_with_nested(aup::Exception{"storage put fail ("+e.code().message()+")"});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put_(const std::filesystem::path& path, std::FILE* f)
    {
        if(path.empty())
        {
            return;
        }

        try
        {
            fs::create_directories(path.parent_path());

            {
                std::FILE* out = fopen(path.string().c_str(), "wb");
                if(!out)
                {
                    throw std::system_error(errno, std::generic_category(), "unable to open "+path.string());
                }
                utils::AtScopeExit se = {[&]{fclose(out);}};

                dbgAssert(f);
                std::rewind(f);
                char buf[1024];
                for(;;)
                {
                    std::size_t s = fread(buf, 1, sizeof(buf), f);
                    if(!s)
                    {
                        break;
                    }

                    if(s != fwrite(buf, 1, s, out))
                    {
                        throw std::system_error(errno, std::generic_category(), "unable to write "+path.string());
                    }

                    if(s != sizeof(buf))
                    {
                        break;
                    }
                }
            }
        }
        catch(const std::system_error& e)
        {
            std::throw_with_nested(aup::Exception{"storage put fail ("+e.code().message()+")"});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has_(const fs::path& path)
    {
        if(path.empty())
        {
            return {};
        }

        if(!fs::exists(path))
        {
            return false;
        }

        if(!fs::is_regular_file(path))
        {
            if(_autoFixIfCan)
            {
                fs::remove_all(path);
            }
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get_(const fs::path& path, uint32 offset, uint32 size)
    {
        if(path.empty())
        {
            return {};
        }

        try
        {
            if(!fs::exists(path))
            {
                return {};
            }

            if(!fs::is_regular_file(path))
            {
                if(_autoFixIfCan)
                {
                    fs::remove_all(path);
                }
                return {};
            }

            Bytes blob;

            {
                std::FILE* in = fopen(path.string().c_str(), "rb");
                if(!in)
                {
                    throw std::system_error(errno, std::generic_category(), "unable to open "+path.string());
                }
                utils::AtScopeExit se = {[&]{fclose(in);}};

                if(fseek(in, offset, SEEK_SET))
                {
                    throw std::system_error(errno, std::generic_category(), "unable to seek "+path.string());
                }

                bytes::Alter a {blob.end()};
                while(size)
                {
                    uint32 bufSize;
                    void* buf = a.prepareWriteBuffer(bufSize);

                    uint32 s = static_cast<uint32>(fread(buf, 1, bufSize, in));
                    if(!s)
                    {
                        break;
                    }

                    a.commitWriteBuffer(s);
                    size -= s;

                    if(s != bufSize)
                    {
                        break;
                    }
                }
            }

            return {std::move(blob)};
        }
        catch(const std::system_error& e)
        {
            std::throw_with_nested(aup::Exception{"storage get fail ("+e.code().message()+")"});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del_(const fs::path& path)
    {
        if(path.empty())
        {
            return {};
        }

        try
        {
            if(!fs::remove_all(path))
            {
                return false;
            }
            fs::path p = path.parent_path();

            fs::path rp = p.lexically_relative(_place);

            while(!rp.empty())
            {
                if(fs::is_empty(p))
                {
                    fs::remove(p);
                }
                else
                {
                    break;
                }

                rp = rp.parent_path();
                p = p.parent_path();
            }

            return true;
        }
        catch(const fs::filesystem_error& e)
        {
            std::throw_with_nested(aup::Exception{"storage del fail: "+e.code().message()});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    fs::path Storage::filePath(const std::string& localPath)
    {
        if(_place.empty())
        {
            return {};
        }

        fs::path res = fs::weakly_canonical(_place / localPath);

        fs::path rp = res.lexically_relative(_place);
        if(rp.empty())
        {
            return {};
        }
        if(".." == *rp.begin())
        {
            return {};
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    fs::path Storage::filePath(const Oid& oid)
    {
        if(_place.empty())
        {
            return {};
        }

        return {
            _place /
            utils::b2h(oid.data()+0, 1) /
            utils::b2h(oid.data()+1, oid.size()-1)};
    }

}
