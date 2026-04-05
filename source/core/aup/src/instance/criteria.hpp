/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/primitives.hpp>

#include <dci/aup/catalog/release.hpp>
#include <dci/aup/catalog/unit.hpp>
#include <dci/aup/catalog/file.hpp>
#include <dci/config.hpp>

namespace dci::aup::instance
{
    namespace criteria
    {
        enum class MatchResult
        {
            null,
            allow,
            deny,
        };

        struct StringEntry  //fnmatch
        {
            bool        _negative{};
            String _pattern;

            void parse(const String& value);
            void parse(const String& value, const auto& sameValueProvider);
            MatchResult match(const String& value) const;
        };

        struct FilePathEntry //fnmatch + fnmPathName|fnmNoEscape
            : StringEntry
        {
            MatchResult match(const String& value) const;
        };

        struct NumRangeEntry
        {
            bool                    _negative{};

            uint32             _min {~uint32{}};
            uint32             _max {};
            Set<uint32>   _concretes;

            void parse(const String& value);
            MatchResult match(const uint32& value) const;
        };

        struct KeyEntry
        {
            bool                                    _negative{};

            bool                                    _any {};
            Set<Array<uint8, 32>>    _concretes;

            void parse(const String& value);
            MatchResult match(const Array<uint8, 32>& value) const;
        };

        struct FileKindEntry
        {
            bool                                _negative{};

            Set<aup::catalog::File::Kind>  _concretes;

            void parse(const String& value);
            MatchResult match(const aup::catalog::File::Kind& value) const;
        };

        template <class Entry>
        struct Entries
        {
            List<Entry> _entries;

            template <class Iter> void parse(const std::pair<Iter, Iter>& range, const String& dflt);
            template <class Iter> void parse(const std::pair<Iter, Iter>& range, const String& dflt, const auto& sameValueProvider);
            MatchResult match(const auto& value) const;
        };
    }

    struct Criteria
    {
        //by release
        criteria::Entries<criteria::FilePathEntry>      _srcBranch;
        criteria::Entries<criteria::StringEntry>        _srcRevision;
        criteria::Entries<criteria::StringEntry>        _platformOs;
        criteria::Entries<criteria::StringEntry>        _platformArch;
        criteria::Entries<criteria::StringEntry>        _compiler;
        criteria::Entries<criteria::StringEntry>        _compilerVersion;
        criteria::Entries<criteria::StringEntry>        _compilerOptimization;
        criteria::Entries<criteria::NumRangeEntry>      _stability;
        criteria::Entries<criteria::StringEntry>        _vendor;
        criteria::Entries<criteria::KeyEntry>           _vendorSign;

        //by unit name
        criteria::Entries<criteria::StringEntry>        _unit;

        //by file
        criteria::Entries<criteria::FileKindEntry>      _fileKind;

    public:
        static Criteria parse(const boost::property_tree::ptree& pt);

        bool match(const aup::catalog::Object* o) const;
        bool match(const aup::catalog::Release* r) const;
        bool match(const aup::catalog::Unit* u) const;
        bool match(const aup::catalog::File* f) const;
    };




    namespace criteria
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Entry>
        MatchResult Entries<Entry>::match(const auto& value) const
        {
            MatchResult res = MatchResult::null;

            for(const Entry& entry : _entries)
            {
                switch(entry.match(value))
                {
                case MatchResult::null:
                    //keep previous value
                    break;
                case MatchResult::allow:
                    res = MatchResult::allow;
                    break;
                case MatchResult::deny:
                    res = MatchResult::deny;
                    break;
                }
            }

            return res;
        }
    }
}
