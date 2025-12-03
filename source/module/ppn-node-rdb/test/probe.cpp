// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/poll.hpp>
#include "ppn/node/rdb.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;

using namespace dci::idl::gen::ppn;
using namespace dci::idl::gen::ppn::node::rdb;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_node_rdb, probe)
{
    Manager* manager = testManager();
    Factory<> f = manager->createService<Factory<>>().value();

    Instance<> i = f->build(List<Feature<>>{}).value();

}
