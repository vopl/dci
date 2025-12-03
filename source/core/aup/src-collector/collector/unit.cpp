// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "unit.hpp"
#include "target.hpp"
#include <iostream>

namespace dci::aup::collector
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Unit::setup(const std::string& key, const std::vector<std::string>& values)
    {
        if("EXTRA_ALLOWED" == key)
        {
            for(const std::string& value : values)
            {
                _extraAllowed.insert(fs::path{value}.lexically_normal().string());
            }
            return true;
        }

        if("SRC_DIR"        == key && emplacePairs(values, _srcDirs)        && checkFirstIsAbsDirs(_srcDirs)        ) return true;
        if("INCLUDE_DIR"    == key && emplacePairs(values, _includeDirs)    && checkFirstIsAbsDirs(_includeDirs)    ) return true;
        if("IDL_DIR"        == key && emplacePairs(values, _idlDirs)        && checkFirstIsAbsDirs(_idlDirs)        ) return true;
        if("CMM_DIR"        == key && emplacePairs(values, _cmmDirs)        && checkFirstIsAbsDirs(_cmmDirs)        ) return true;

        if("SRC_FILE"       == key && emplacePairs(values, _srcFiles)       && checkFirstIsAbsFiles(_srcFiles)      ) return true;
        if("INCLUDE_FILE"   == key && emplacePairs(values, _includeFiles)   && checkFirstIsAbsFiles(_includeFiles)  ) return true;
        if("IDL_FILE"       == key && emplacePairs(values, _idlFiles)       && checkFirstIsAbsFiles(_idlFiles)      ) return true;
        if("CMM_FILE"       == key && emplacePairs(values, _cmmFiles)       && checkFirstIsAbsFiles(_cmmFiles)      ) return true;

        return Meta::setup(key, values);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Target* Unit::getTarget(const std::string& name, bool addIfMissing)
    {
        if(addIfMissing)
        {
            return &_targets.emplace(
                        std::piecewise_construct_t{},
                        std::tuple{name},
                        std::tuple{this, name}).first->second;
        }

        auto iter = _targets.find(name);
        if(_targets.end() != iter)
        {
            return &iter->second;
        }

        return {};
    }
}
