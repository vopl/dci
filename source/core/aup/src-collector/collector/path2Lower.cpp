// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "path2Lower.hpp"
#ifdef _WIN32
#   include <windows.h>
#endif

namespace dci::aup::collector
{
    namespace fs = std::filesystem;

#ifdef _WIN32
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::filesystem::path path2Lower(const std::filesystem::path& value)
    {
        std::wstring str{value.native()};
        CharLowerW(str.data());
        return {str};
    }
#endif
}
