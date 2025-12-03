// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>

#include <filesystem>

#include "im/storage.hpp"
#include "im/errorInfo.hpp"

#include "prs/config.hpp"
#include "prs/exec.hpp"

using namespace dci::idl;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, parser1)
{
    im::Storage ims;

    prs::Config cfg;
    cfg._includeDirectories.push_back(TESTDIR);

    std::string idlFile = "parser1.idl";

    std::vector<im::ErrorInfo> errors;
    std::vector<std::string> sources;

    im::ast::Scope rootScope = prs::exec(idlFile, cfg, errors, sources);

    EXPECT_TRUE(!!rootScope);
    EXPECT_TRUE(errors.empty());
    for(const im::ErrorInfo& err : errors)
    {
        std::cerr<<err.toString()<<std::endl;
    }

    if(rootScope)
    {
        ims.add(rootScope);
        bool b = ims.commit(errors);
        EXPECT_TRUE(b);
        EXPECT_TRUE(errors.empty());
        for(const im::ErrorInfo& err : errors)
        {
            std::cerr<<err.toString()<<std::endl;
        }
    }
}
