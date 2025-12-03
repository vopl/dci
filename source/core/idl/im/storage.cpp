// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "storage.hpp"

#include "proc/nestedScopeExpander.hpp"
#include "proc/ownerIndexer.hpp"
#include "proc/scopeMerger.hpp"
#include "proc/nameChecker.hpp"
#include "proc/scopedNameResolver.hpp"
#include "proc/scopedNameOrderChecker.hpp"
#include "proc/oppositeSynthesizer.hpp"
#include "proc/signer4Name.hpp"
#include "proc/signer4Layout.hpp"

namespace dci::idl::im
{
    void Storage::add(const ast::Scope& root)
    {
        dbgAssert(root);

        if(!_root)
        {
            _root = root;
        }
        else
        {
            _root->decls.insert(_root->decls.end(), root->decls.begin(), root->decls.end());
        }
    }

    void Storage::addSource(const std::string& source)
    {
        _sources.emplace_back(source);
    }

    bool Storage::commit(std::vector<ErrorInfo>& errors)
    {
        dbgAssert(errors.empty());
        if(!errors.empty())
        {
            return false;
        }

        if(!_root)
        {
            errors.emplace_back(ErrorInfo {
                                  "empty input",
                                  PosInSources{}});
            return false;
        }

        //expand nested scopes
        proc::NestedScopeExpander().exec(_root);

        //index owners
        proc::OwnerIndexer().exec(_root);

        //merge scopes
        proc::ScopeMerger().exec(_root);

        //check names uniqueness
        proc::NameChecker(errors).exec(_root);
        if(!errors.empty())
        {
            return false;
        }

        //make synthetics for opposite interfaces
        proc::OppositeSynthesizer oppositeSynthesizer;
        oppositeSynthesizer.initiate(_root);

        //resolve typeUse.scopedName
        proc::ScopedNameResolver(errors).exec(_root);
        if(!errors.empty())
        {
            return false;
        }

        //check scoped names vs declarations ordering
        proc::ScopedNameOrderChecker(errors).exec(_root);
        if(!errors.empty())
        {
            return false;
        }

        oppositeSynthesizer.finalize(_root);

        //sign
        proc::Signer4Name{}.exec(_root);
        proc::Signer4Layout{}.exec(_root);

        return true;
    }

    const ast::Scope& Storage::root() const
    {
        return _root;
    }

    const std::vector<std::string> Storage::sources() const
    {
        return _sources;
    }
}
