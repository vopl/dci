// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "config.hpp"
#include "im/errorInfo.hpp"
#include "im/ast/scope.hpp"

#include <vector>
#include <string>

namespace dci::idl::prs
{
    im::ast::Scope exec(
            const std::string& fileName,
            const Config& cfg,
            std::vector<im::ErrorInfo>& errors,
            std::vector<std::string>& sourceFilesParsed);

    std::vector<im::ast::Scope> exec(
            const std::vector<std::string>& fileNames,
            const Config& cfg,
            std::vector<im::ErrorInfo>& errors,
            std::vector<std::string>& sourceFilesParsed);
}
