// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/host/test.hpp>

namespace dci::host
{
    extern TestStage g_testStage;
    extern Manager* g_testManager;

    TestStage g_testStage = TestStage::null;
    Manager* g_testManager = nullptr;

    TestStage testStage()
    {
        return g_testStage;
    }

    Manager* testManager()
    {
        return g_testManager;
    }

}
