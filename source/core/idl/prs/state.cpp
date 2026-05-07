/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "state.hpp"
#include "error.hpp"
#include "grammar.hpp"

#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/dbg.hpp>
#include <fstream>
#include <filesystem>

namespace dci::idl::prs
{
    namespace fs = std::filesystem;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    State::State(const Config& cfg, std::vector<im::ErrorInfo>& errors, std::vector<std::string>& sourceFilesParsed)
        : _cfg(cfg)
        , _errors(errors)
        , _sourceFilesParsed(sourceFilesParsed)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    State::~State()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void State::pushError(const std::string& msg, const Iterator& pos)
    {
        _errors.push_back({msg, pos2Im(pos)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    im::ast::Scope State::process(const std::string& fileName, bool local, bool once)
    {
        Iterator pos = _sourcesStack.empty() ? Iterator{} : _sourcesStack.back()._pos;

        std::string resolverErrorMessage;
        std::string resolvedFileName = resolveFileName(local, fileName, resolverErrorMessage);
        if(resolvedFileName.empty())
        {
            pushError("'" + fileName + "': " + resolverErrorMessage, pos);
            return {};
        }

        if(once && _sourceFilesParsed.end() != std::find(_sourceFilesParsed.begin(), _sourceFilesParsed.end(), resolvedFileName))
        {
            //already parsed
            return {};
        }
        _sourceFilesParsed.emplace_back(resolvedFileName);

        //check cyclic
        if(_sourcesStack.end() != std::find_if(_sourcesStack.begin(), _sourcesStack.end(), [&](const SourcePos& next){ return next._source->_file == resolvedFileName;}))
        {
            pushError("'" + resolvedFileName + "': cyclic inclusion", pos);
            return {};
        }

        //load content
        SourcePtr source;
        {
            std::ifstream ifstr{resolvedFileName};
            if(!ifstr)
            {
                pushError(std::string{"'"} + resolvedFileName + "': unable to read (" + strerror(errno) + ")", pos);
                return {};
            }

            source = std::make_shared<Source>();
            source->_file = resolvedFileName;

            ifstr.seekg(0, std::ios::end);
            source->_content.resize(static_cast<std::size_t>(ifstr.tellg()));
            ifstr.seekg(0, std::ios::beg);

            ifstr.read(source->_content.data(), static_cast<std::streamsize>(source->_content.size()));
        }

        if(source->_content.empty())
        {
            // пустой исходник - в нем пустой глобальный скоуп
            return std::make_shared<im::ast::SScope>();
        }

        _sourcesStack.push_back(SourcePos{source, &source->_content[0]});
        utils::AtScopeExit fnr{[&]
        {
            _sourcesStack.pop_back();
        }};

        const Iterator begin{&source->_content.front()};
        const Iterator end{&source->_content.back()+1};

        im::ast::Scope res;
        try
        {
            Iterator iter{begin};
            bool r = boost::spirit::x3::phrase_parse(
                iter,
                end,
                boost::spirit::x3::with<State>(*this)[grammar::file],
                grammar::skipper,
                res);

            if(!r)
            {
                pushError(std::string{"fail"}, iter);
                return {};
            }

            if(iter != end)
            {
                pushError(std::string{"unexpected input"}, iter);
                return {};
            }
        }
        catch(const Error& e)
        {
            pushError(e.what(), e.pos());
            return {};
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void State::storePos(Iterator pos)
    {
        dbgAssert(!_sourcesStack.empty());
        dbgAssert(&_sourcesStack.back()._source->_content.front() <= pos && pos <= &_sourcesStack.back()._source->_content.back()+1);
        _sourcesStack.back()._pos = pos;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    im::PosInSources State::pos2Im(Iterator pos)
    {
        im::PosInSources res;

        if(!pos)
        {
            return res;
        }

        dbgAssert(!_sourcesStack.empty());
        dbgAssert(&_sourcesStack.back()._source->_content.front() <= pos && pos <= &_sourcesStack.back()._source->_content.back()+1);

        auto pushOne = [&](const Source& src, Iterator pos)
        {
            dbgAssert(&src._content.front() <= pos && pos <= &src._content.back()+1);

            int line = 1;
            int column = 1;
            char prev = '\0';

            for(Iterator chIter = &src._content.front(); chIter != pos; ++chIter)
            {
                switch(*chIter)
                {
                case '\r':
                    if(prev != '\n')
                    {
                        ++line;
                        column = 1;
                    }
                    break;
                case '\n':
                    if(prev != '\r')
                    {
                        ++line;
                        column = 1;
                    }
                    break;
                default:
                    ++column;
                    break;
                }

                prev = *chIter;
            }

            res.add(src._file, line, column);
        };

        for(std::size_t idx{}; idx+1 < _sourcesStack.size(); ++idx)
        {
            pushOne(*_sourcesStack[idx]._source, _sourcesStack[idx]._pos);
        }
        pushOne(*_sourcesStack.back()._source, pos);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string State::resolveFileName(bool local, const std::string& in, std::string& errorMessage)
    {
        try
        {
            fs::path p{in};
            fs::path result{};
            if(p.is_absolute())
            {
                result = p;
                if(!fs::exists(result))
                {
                    errorMessage = "not found";
                    return {};
                }
            }

            if(result.empty() && local && !_sourcesStack.empty())
            {
                fs::path base = fs::absolute(_sourcesStack.back()._source->_file).remove_filename();
                if(fs::path candidate{base / p}; fs::exists(candidate))
                {
                    result = std::move(candidate);
                }
            }

            if(result.empty())
            {
                for(const std::string& base : _cfg._includeDirectories)
                {
                    if(fs::path candidate{base / p}; fs::exists(candidate))
                    {
                        result = std::move(candidate);
                        break;
                    }
                }
            }

            if(result.empty() || !fs::exists(result))
            {
                errorMessage = "not found";
                return {};
            }

            if(fs::is_symlink(result))
            {
                result = fs::read_symlink(result);
            }

            if(fs::is_regular_file(result))
            {
                result = result.lexically_normal();
                return result.generic_string();
            }

            errorMessage = "is not a regular file";
            return {};
        }
        catch(const fs::filesystem_error& err)
        {
            errorMessage = err.code().message();
            return {};
        }

        dbgFatal("never here");
        return {};
    }
}
