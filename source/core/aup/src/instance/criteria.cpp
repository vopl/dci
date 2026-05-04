/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "criteria.hpp"
#include <dci/integration/info.hpp>
#include <dci/aup/exception.hpp>
#include <dci/aup/catalog/identify.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/fnmatch.hpp>
#include <dci/exception.hpp>
#include <dci/logger.hpp>

namespace std
{
    template <class Iter>
    Iter begin(const std::pair<Iter, Iter>& range)
    {
        return range.first;
    }

    template <class Iter>
    Iter end(const std::pair<Iter, Iter>& range)
    {
        return range.second;
    }
}

namespace dci::aup::instance
{
    namespace criteria
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void StringEntry::parse(const String& value)
        {
            return parse(value, nullptr);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void StringEntry::parse(const String& value, const auto& sameValueProvider)
        {
            std::string_view sv{value};

            if(sv.empty()) throw aup::Exception{"criteria parser: empty stringEntry value is not allowed"};

            if(sv[0] == '-')
            {
                _negative = true;
                sv.remove_prefix(1);
            }

            if(sv.empty()) throw aup::Exception{"criteria parser: bad stringEntry value: " + value};

            if constexpr(std::is_invocable_v<decltype(sameValueProvider)>)
            {
                if(sv == "same")
                {
                    _pattern = sameValueProvider();
                }
                else
                {
                    _pattern = sv;
                }
            }
            else
            {
                _pattern = sv;
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        MatchResult StringEntry::match(const String& value) const
        {
            if(utils::fnmatch(_pattern.data(), value.data()))
            {
                return _negative ? MatchResult::deny : MatchResult::allow;
            }

            return MatchResult::null;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        MatchResult FilePathEntry::match(const String& value) const
        {
            if(utils::fnmatch(_pattern.data(), value.data(), utils::fnmPathName | dci::utils::fnmNoEscape))
            {
                return _negative ? MatchResult::deny : MatchResult::allow;
            }

            return MatchResult::null;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void NumRangeEntry::parse(const String& value)
        {
            std::string s{value};

            if(s.empty()) throw aup::Exception{"criteria parser: empty numRangeEntry value is not allowed"};

            if(s[0] == '-')
            {
                _negative = true;
                s.erase(s.begin());
            }

            if(s.empty()) throw aup::Exception{"criteria parser: bad numRangeEntry value: " + value};

            try
            {
                if("*" == s)
                {
                    _min = 0;
                    _max = ~uint32{};
                }
                else
                {
                    auto dashPos = s.find("-");

                    if(std::string::npos == dashPos)
                    {
                        //num
                        _concretes.insert(static_cast<uint32>(std::stol(s)));
                    }
                    else
                    {
                        //min-max
                        _min = static_cast<uint32>(std::stol(s.substr(0, dashPos)));
                        _max = static_cast<uint32>(std::stol(s.substr(dashPos+1)));
                    }
                }
            }
            catch(...)
            {
                std::throw_with_nested(aup::Exception{"criteria parser: bad numRangeEntry value: " + value});
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        MatchResult NumRangeEntry::match(const uint32& value) const
        {
            if((value >= _min && value <= _max) || _concretes.count(value))
            {
                return _negative ? MatchResult::deny : MatchResult::allow;
            }

            return MatchResult::null;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void KeyEntry::parse(const String& value)
        {
            std::string s{value};

            if(s.empty()) throw aup::Exception{"criteria parser: empty keyEntry value is not allowed"};

            if(s[0] == '-')
            {
                _negative = true;
                s.erase(s.begin());
            }

            if(s.empty()) throw aup::Exception{"criteria parser: bad keyEntry value: " + value};

            if("*" == s)
            {
                _any = true;
                return;
            }

            Array<uint8, 32> b;
            if(b.size()*2 != s.size() || !utils::h2b(s.data(), s.size(), b.data()))
            {
                throw aup::Exception{"criteria parser: bad keyEntry value: "+s};
            }

            _concretes.insert(b);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        MatchResult KeyEntry::match(const Array<uint8, 32>& value) const
        {
            if(_any || _concretes.count(value))
            {
                return _negative ? MatchResult::deny : MatchResult::allow;
            }

            return MatchResult::null;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void FileKindEntry::parse(const String& value)
        {
            std::string s{value};

            if(s.empty()) throw aup::Exception{"criteria parser: empty fileKindEntry value is not allowed"};

            if(s[0] == '-')
            {
                _negative = true;
                s.erase(s.begin());
            }

            if(s.empty()) throw aup::Exception{"criteria parser: bad fileKindEntry value: " + value};

            if("*" == s)
            {
                _concretes.insert(aup::catalog::File::Kind::runtime);
                _concretes.insert(aup::catalog::File::Kind::resource);
                _concretes.insert(aup::catalog::File::Kind::rdep);
                _concretes.insert(aup::catalog::File::Kind::test);
                _concretes.insert(aup::catalog::File::Kind::debug);
                _concretes.insert(aup::catalog::File::Kind::bdep);
                _concretes.insert(aup::catalog::File::Kind::include);
                _concretes.insert(aup::catalog::File::Kind::idl);
                _concretes.insert(aup::catalog::File::Kind::cmm);
                _concretes.insert(aup::catalog::File::Kind::src);
            }
            else if("dev" == s)
            {
                _concretes.insert(aup::catalog::File::Kind::runtime);
                _concretes.insert(aup::catalog::File::Kind::resource);
                _concretes.insert(aup::catalog::File::Kind::rdep);
                _concretes.insert(aup::catalog::File::Kind::test);
                _concretes.insert(aup::catalog::File::Kind::debug);
                _concretes.insert(aup::catalog::File::Kind::bdep);
                _concretes.insert(aup::catalog::File::Kind::include);
                _concretes.insert(aup::catalog::File::Kind::idl);
                _concretes.insert(aup::catalog::File::Kind::cmm);
            }
            else if("minimal" == s)
            {
                _concretes.insert(aup::catalog::File::Kind::runtime);
                _concretes.insert(aup::catalog::File::Kind::resource);
                _concretes.insert(aup::catalog::File::Kind::rdep);
            }
            else if("runtime"   == s) _concretes.insert(aup::catalog::File::Kind::runtime);
            else if("resource"  == s) _concretes.insert(aup::catalog::File::Kind::resource);
            else if("rdep"      == s) _concretes.insert(aup::catalog::File::Kind::rdep);
            else if("test"      == s) _concretes.insert(aup::catalog::File::Kind::test);
            else if("debug"     == s) _concretes.insert(aup::catalog::File::Kind::debug);
            else if("bdep"      == s) _concretes.insert(aup::catalog::File::Kind::bdep);
            else if("include"   == s) _concretes.insert(aup::catalog::File::Kind::include);
            else if("idl"       == s) _concretes.insert(aup::catalog::File::Kind::idl);
            else if("cmm"       == s) _concretes.insert(aup::catalog::File::Kind::cmm);
            else if("src"       == s) _concretes.insert(aup::catalog::File::Kind::src);
            else
            {
                throw aup::Exception{"criteria parser: bad fileKindEntry value: " + value};
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        MatchResult FileKindEntry::match(const aup::catalog::File::Kind& value) const
        {
            if(_concretes.count(value))
            {
                return _negative ? MatchResult::deny : MatchResult::allow;
            }

            return MatchResult::null;
        }

        namespace
        {
            auto entriesParser = [](auto& entries, const auto& range, const String& dflt, const auto& oneParser)
            {
                auto parseOne = [&](const String& v)
                {
                    typename std::remove_reference_t<decltype(entries)>::value_type e;
                    oneParser(e, v);
                    entries.emplace_back(std::move(e));
                };

                for(const auto& kv : range)
                {
                    parseOne(kv.second.data());
                }

                if(range.first == range.second)
                {
                    parseOne(dflt);
                }
            };
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Entry>
        template <class Iter>
        void Entries<Entry>::parse(const std::pair<Iter, Iter>& range, const String& dflt)
        {
            auto oneParser = [](auto& e, const String& v)
            {
                return e.parse(v);
            };

            return entriesParser(_entries, range, dflt, oneParser);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Entry>
        template <class Iter>
        void Entries<Entry>::parse(const std::pair<Iter, Iter>& range, const String& dflt, const auto& sameValueProvider)
        {
            auto oneParser = [&](auto& e, const String& v)
            {
                return e.parse(v, sameValueProvider);
            };

            return entriesParser(_entries, range, dflt, oneParser);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Criteria Criteria::parse(const boost::property_tree::ptree& pt)
    {
        instance::Criteria res;

        res._srcBranch              .parse(pt.equal_range("srcBranch"),             "same", &dci::integration::info::srcBranch);
        res._srcRevision            .parse(pt.equal_range("srcRevision"),           "*",    &dci::integration::info::srcRevision);
        res._platformOs             .parse(pt.equal_range("platformOs"),            "same", &dci::integration::info::platformOs);
        res._platformArch           .parse(pt.equal_range("platformArch"),          "same", &dci::integration::info::platformArch);
        res._compiler               .parse(pt.equal_range("compiler"),              "same", &dci::integration::info::compiler);
        res._compilerVersion        .parse(pt.equal_range("compilerVersion"),       "*",    &dci::integration::info::compilerVersion);
        res._compilerOptimization   .parse(pt.equal_range("compilerOptimization"),  "same", &dci::integration::info::compilerOptimization);
        res._stability              .parse(pt.equal_range("stability"),             "*");
        res._vendor                 .parse(pt.equal_range("vendor"),                "same", &dci::integration::info::vendor);
        res._vendorSign             .parse(pt.equal_range("vendorSign"),            "*");
        res._unit                   .parse(pt.equal_range("unit"),                  "*");
        res._fileKind               .parse(pt.equal_range("fileKind"),              "minimal");

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Criteria::match(const aup::catalog::Object* o) const
    {
        switch(o->type())
        {
        case aup::catalog::Object::Type::release:
            return match(aup::catalog::objectPtrCast<aup::catalog::Release>(o));
        case aup::catalog::Object::Type::unit:
            return match(aup::catalog::objectPtrCast<aup::catalog::Unit>(o));
        case aup::catalog::Object::Type::file:
            return match(aup::catalog::objectPtrCast<aup::catalog::File>(o));
        default:
            break;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Criteria::match(const aup::catalog::Release* r) const
    {
        if(criteria::MatchResult::allow != _srcBranch              .match(r->_srcBranch)               ) return false;
        if(criteria::MatchResult::allow != _srcRevision            .match(r->_srcRevision)             ) return false;
        if(criteria::MatchResult::allow != _platformOs             .match(r->_platformOs)              ) return false;
        if(criteria::MatchResult::allow != _platformArch           .match(r->_platformArch)            ) return false;
        if(criteria::MatchResult::allow != _compiler               .match(r->_compiler)                ) return false;
        if(criteria::MatchResult::allow != _compilerVersion        .match(r->_compilerVersion)         ) return false;
        if(criteria::MatchResult::allow != _compilerOptimization   .match(r->_compilerOptimization)    ) return false;
        if(criteria::MatchResult::allow != _stability              .match(r->_stability)               ) return false;
        if(criteria::MatchResult::allow != _vendor                 .match(r->_vendor)                  ) return false;
        if(criteria::MatchResult::allow != _vendorSign             .match(r->_vendorSign)              ) return false;

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Criteria::match(const aup::catalog::Unit* u) const
    {
        return criteria::MatchResult::allow == _unit.match(u->_name);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Criteria::match(const aup::catalog::File* f) const
    {
        return criteria::MatchResult::allow == _fileKind.match(f->_kind);
    }
}
