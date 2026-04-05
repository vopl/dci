/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <fstream>
#include <system_error>
#include <stack>
#include <sstream>

namespace dci::idl::gen
{
    class Out;
    Out& el(Out&);
    Out& indent(Out&);
    Out& undent(Out&);


    class Out
    {
    public:
        Out();
        ~Out();

        void open();
        void open(const std::string& fname);
        void close();

        void setIndent(const std::string& v);

        void push();
        std::string pop(bool commit);

        operator bool();

        Out& operator<<(Out&(*)(Out&));
        Out& operator<<(std::ostream&(*v)(std::ostream&));

        template <class T>
        Out& operator<<(T v);

    public:
        struct ScopedIndentRaii
        {
            ScopedIndentRaii();
            ScopedIndentRaii(Out* target, const std::string& prefix, const std::string& suffix);
            ScopedIndentRaii(const ScopedIndentRaii&) = delete;
            ScopedIndentRaii(ScopedIndentRaii&&);
            ~ScopedIndentRaii();

            void operator=(const ScopedIndentRaii&) = delete;
            void operator=(ScopedIndentRaii&&);

        private:
            Out * _target {};
            std::string _suffix;
        };

        struct ContentPrefixRaii
        {
            ContentPrefixRaii();
            ContentPrefixRaii(Out* target, const std::string& prefix);
            ContentPrefixRaii(const ContentPrefixRaii&) = delete;
            ContentPrefixRaii(ContentPrefixRaii&&);
            ~ContentPrefixRaii();

            void operator=(const ContentPrefixRaii&) = delete;
            void operator=(ContentPrefixRaii&&);

        private:
            Out * _target {};
        };

    public:
        ScopedIndentRaii scope(const std::string& prefix="{", const std::string& suffix="}");
        ContentPrefixRaii contentPrefix(const std::string& prefix="//");

        void pushContentPrefix(const std::string& prefix="//");
        void popContentPrefix();

    private:
        void flushNewLine();

    private:
        std::string                     _fname;
        std::ofstream                   _fs;
        std::ostream *                  _s {nullptr};

        struct Lining
        {
            std::size_t             _indentLevel{};
            bool                    _isNewLine{};
            std::string             _indentPad{"    "};
            std::stack<std::string> _contentPrefix;
        };

        Lining _lining;

        struct Level : Lining
        {
            std::stringstream       _stringstream{std::ios_base::out};
        };

        std::stack<Level>   _levels;

        std::ostream* _currentStream {nullptr};
        Lining* _currentLining {nullptr};

        friend Out& el(Out&);
        friend Out& indent(Out&);
        friend Out& undent(Out&);
    };


    template <class T>
    Out& Out::operator<<(T v)
    {
        flushNewLine();

        (*_currentStream) << v;
        if(!*_currentStream)
        {
            throw std::system_error(std::error_code{errno, std::generic_category()}, "unable to write file "+_fname);
        }
        _currentStream->flush();

        return *this;
    }

}
