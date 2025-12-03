// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "exec.hpp"
#include "state.hpp"

namespace dci::idl::prs
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    im::ast::Scope exec(
            const std::string& fileName,
            const Config& cfg,
            std::vector<im::ErrorInfo>& errors,
            std::vector<std::string>& sourceFilesParsed)
    {
        State state{cfg, errors, sourceFilesParsed};
        return state.process(fileName);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::vector<im::ast::Scope> exec(
            const std::vector<std::string>& fileNames,
            const Config& cfg,
            std::vector<im::ErrorInfo>& errors,
            std::vector<std::string>& sourceFilesParsed)
    {
        std::vector<im::ast::Scope> res;
        State state{cfg, errors, sourceFilesParsed};

        for(const std::string& fileName : fileNames)
        {
            res.emplace_back(state.process(fileName));
        }

        return res;
    }
}
