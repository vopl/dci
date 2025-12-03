// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../ast.hpp"
#include <map>

namespace dci::idl::im::proc
{
    using namespace ast;

    class ScopeMerger
    {
    public:
        void exec(const Scope& s)
        {
            _mode = Mode::initiate;
            process(s);

            _mode = Mode::finalize;
            process(s);
        }

    private:
        enum class Mode
        {
            initiate,
            finalize
        } _mode = Mode::initiate;

        std::map<std::string, std::vector<SScope*>> _fqn2Siblings;

    private:
        void process(const Scope& v)
        {
            std::string fqn = v->prepareFullScopedName()->toString();

            if(Mode::initiate == _mode)
                _fqn2Siblings[fqn].emplace_back(v.get());
            else
                v->siblings = _fqn2Siblings[fqn];

            for(const Decl& decl : v->decls)
                boost::apply_visitor([this](auto&& v){process(v);}, decl);
        }

        void process(auto&&) {}
    };
}
