// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/test/entryPoint.hpp>

namespace dci::test
{
    API_DCI_TEST int entryPoint(const std::vector<std::string>& argv)
    {
        int c_argc = static_cast<int>(argv.size());
        std::vector<char*> c_argv;
        c_argv.reserve(argv.size());

        std::vector<std::string> argvCopy(argv);

        for(std::string& arg : argvCopy)
        {
            c_argv.push_back(arg.data());
        }
        c_argv.push_back(nullptr);

        testing::InitGoogleTest(&c_argc, c_argv.data());
        int res = RUN_ALL_TESTS();

        return res;
    }
}
