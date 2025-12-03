// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include "absAndRel.hpp"

namespace dci::aup::collector
{
    struct Meta
    {
        Meta(Meta* parent, const std::string& name);
        virtual ~Meta() = default;

        virtual bool setup(const std::string& key, const std::vector<std::string>& values);

    protected:
        static bool emplacePairs(const std::vector<std::string>& values, auto& dst);

        static bool checkIsAbsDir(const std::filesystem::path& path);
        static bool checkIsAbsFile(const std::filesystem::path& path);
        static bool checkIsAbsDir(const std::string& path);
        static bool checkIsAbsFile(const std::string& path);

        static bool checkFirstIsAbsDirs(const auto& pairs);
        static bool checkFirstIsAbsFiles(const auto& pairs);
        static bool checkIsAbsFiles(const auto& paths);

    public:
        Meta* _parent;

    public:
        std::string                         _name;

        std::set<AbsAndRel>                 _dirMapping;

        std::set<AbsAndRel>                 _resourceDirs;
        std::set<AbsAndRel>                 _resourceFiles;

        std::map<std::string, std::string>  _syslibMapTo;
        std::set<std::string>               _syslibIgnore;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::emplacePairs(const std::vector<std::string>& values, auto& dst)
    {
        for(std::size_t i{}; i<values.size(); i+=2)
        {
            std::string key = values[i];
            std::string val = values.size() > i+1 ? values[i+1] : std::string{};

            dst.emplace(
                        fs::path{key}.lexically_normal().string(),
                        fs::path{val}.lexically_normal().string());
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkFirstIsAbsDirs(const auto& pairs)
    {
        for(const auto& pair : pairs)
        {
            if(!checkIsAbsDir(std::get<0>(pair)))
            {
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkFirstIsAbsFiles(const auto& pairs)
    {
        for(const auto& pair : pairs)
        {
            if(!checkIsAbsFile(std::get<0>(pair)))
            {
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsFiles(const auto& paths)
    {
        for(const auto& path : paths)
        {
            if(!checkIsAbsFile(path))
            {
                return false;
            }
        }

        return true;
    }

}
