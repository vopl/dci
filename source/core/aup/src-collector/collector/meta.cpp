// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "meta.hpp"
#include <iostream>
#include <cassert>

namespace dci::aup::collector
{
    Meta::Meta(Meta* parent, const std::string& name)
        : _parent{parent}
        , _name{name}
    {
    }

    bool Meta::setup(const std::string& key, const std::vector<std::string>& values)
    {
        if("DIR_MAPPING"    == key && emplacePairs(values, _dirMapping)     && checkFirstIsAbsDirs(_dirMapping)     ) return true;
        if("RESOURCE_DIR"   == key && emplacePairs(values, _resourceDirs)   && checkFirstIsAbsDirs(_resourceDirs)   ) return true;
        if("RESOURCE_FILE"  == key && emplacePairs(values, _resourceFiles)  && checkFirstIsAbsFiles(_resourceFiles) ) return true;
        if("SYSLIB_MAPTO"   == key && emplacePairs(values, _syslibMapTo)) return true;

        if("SYSLIB_IGNORE" == key)
        {
            for(const std::string& value : values)
            {
                _syslibIgnore.insert(fs::path{value}.lexically_normal().string());
            }
            return true;
        }

        std::cerr << "bad meta payload: " << key << "[";
        for(const std::string& v : values)
        {
            std::cerr << v << ";";
        }
        std::cerr << "]"<<std::endl;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsDir(const std::filesystem::path& path)
    {
        if(!path.is_absolute() || !fs::is_directory(path))
        {
            std::cout << "not a normal absolute path to a directory: " << path << std::endl;
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsFile(const std::filesystem::path& path)
    {
        if(!path.is_absolute() || !fs::is_regular_file(path))
        {
            std::cout << "not a normal absolute path to a file: " << path << std::endl;
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsDir(const std::string& path)
    {
        return checkIsAbsDir(fs::path{path});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsFile(const std::string& path)
    {
        return checkIsAbsFile(fs::path{path});
    }
}
