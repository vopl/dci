// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "ast.hpp"
#include "errorInfo.hpp"

namespace dci::idl::im
{
    class Storage
    {
    public:
        void add(const ast::Scope& root);
        void addSource(const std::string& source);
        bool commit(std::vector<ErrorInfo>& errors);

        const ast::Scope& root() const;
        const std::vector<std::string> sources() const;

    private:
        ast::Scope                  _root;
        std::vector<std::string>    _sources;
    };
}
