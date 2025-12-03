// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "dll.hpp"
#include <map>

namespace dci::host
{
    boost::dll::shared_library& dll(const std::string path)
    {
        static std::map<std::string, boost::dll::shared_library> all;
        boost::dll::shared_library& sl = all[path];
        if(!sl.is_loaded())
            sl.load(path, boost::dll::load_mode::rtld_now | boost::dll::load_mode::rtld_local /*| boost::dll::load_mode::rtld_deepbind*/);
        return sl;
    }
}
