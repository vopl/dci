// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
