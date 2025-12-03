// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "config.hpp"
#include "im/errorInfo.hpp"
#include "im/ast.hpp"
#include "iterator.hpp"

namespace dci::idl::prs
{
    class State
    {
    public:
        State(const Config& cfg, std::vector<im::ErrorInfo>& errors, std::vector<std::string>& sourceFilesParsed);
        ~State();

        im::ast::Scope process(const std::string& fileName, bool once = true);
        void storePos(Iterator pos);
        im::PosInSources pos2Im(Iterator pos);

        void pushError(const std::string& msg, const Iterator& pos);

    private:
        std::string resolveFileName(const std::string& in, std::string& errorMessage);

    private:
        const Config&                   _cfg;
        std::vector<im::ErrorInfo>&     _errors;
        std::vector<std::string>&       _sourceFilesParsed;

        struct Source
        {
            std::string         _file;
            std::vector<char>   _content{};
        };
        using SourcePtr = std::shared_ptr<Source>;

        struct SourcePos
        {
            SourcePtr   _source;
            Iterator    _pos;
        };

        std::deque<SourcePos> _sourcesStack;
    };
}
