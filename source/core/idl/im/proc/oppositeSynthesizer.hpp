// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../ast.hpp"

namespace dci::idl::im::proc
{
    using namespace ast;

    class OppositeSynthesizer
    {
    public:
        void initiate(const Scope& s)
        {
            _mode = Mode::initiate;
            process(s.get());
        }

        void finalize(const Scope& s)
        {
            _mode = Mode::finalize;
            process(s.get());
        }

    private:
        enum class Mode
        {
            initiate,
            finalize
        } _mode = Mode::initiate;

    private:
        void process(SScope* v)
        {
            for(SInterface* interface : v->interfaces)
                process(interface);

            for(SScope* scope : v->scopes)
                process(scope);
        }

        void process(SInterface* i1)
        {
            if(Mode::initiate == _mode)
            {
                Interface i2 = std::make_shared<SInterface>();
                i1->opposite = i2.get();
                i1->syntheticHolder = i2;

                i2->isPrimary = false;
                i2->opposite = i1;
                i2->name = i1->name;
                i2->owner = i1->owner;

                i2->methods.reserve(i1->methods.size());
                for(const Method& m1 : i1->methods)
                {
                    Method m2 = std::make_shared<SMethod>(*m1);
                    m2->direction = MethodDirection::out == m1->direction ?
                                        MethodDirection::in :
                                        MethodDirection::out;
                    m2->owner = i2.get();
                    for(const MethodParam& mp2 : m2->query)
                        mp2->owner = m2.get();

                    i2->methods.emplace_back(std::move(m2));
                }
            }

            if(Mode::finalize == _mode)
            {
                Interface i2 = i1->syntheticHolder;

                i2->bases.reserve(i1->bases.size());
                for(const InterfaceBase& b1 : i1->bases)
                {
                    InterfaceBase b2 = std::make_shared<SInterfaceBase>();
                    b2->scopedName = std::make_shared<SScopedName>(*b1->scopedName);
                    b2->scopedName->asDecl = i2.get();
                    b2->scopedName->asScopedEntry = i2.get();
                    b2->instance = b1->instance->opposite;

                    i2->bases.emplace_back(std::move(b2));
                }
            }
        }
    };
}
