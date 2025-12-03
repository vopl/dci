// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../ast.hpp"

namespace dci::idl::im::proc
{
    using namespace ast;

    class Signer4Name
    {
    public:
        Signer4Name();
        void exec(Scope& s);

    private:
        template <class ScopeEntry> void process(ScopeEntry* e);

        void visit(SScope* v);
    };
}
