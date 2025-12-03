// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <boost/dll.hpp>

namespace dci::host
{
    boost::dll::shared_library& dll(const std::string path);
}
