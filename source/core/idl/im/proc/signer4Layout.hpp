// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../ast.hpp"
#include <map>

namespace dci::idl::im::proc
{
    using namespace ast;

    class Signer4Layout
    {
    public:
        Signer4Layout();
        void exec(Scope& s);

        struct Node;
        struct Dep
        {
            std::string _kind;
            std::size_t _index{};
            Node*       _target{};
        };

        struct Node
        {
            Sign*                   _result{};
            std::deque<Dep>         _deps;

            std::array<uint8_t, 32> _rise;

            bool                    _finalized{};
        };

        std::map<void*, Node>   _nodes;

    private:
        template <class AstNode>
        bool emplace(AstNode* astNode, Node*& node);

        void finalize(const std::deque<Node*>& loop);

        template <class Part>
        void addPart(Node* node, std::string_view tag, const Part& part);

        template <class Part>
        void addPart(Node* node, std::string_view tag, std::size_t idx, const std::shared_ptr<Part>& part);

        template <class Part>
        void addPart(Node* node, std::string_view tag, std::size_t idx, const Part& part);

    private:
        Node* init(SScope* v);

        Node* init(SInterface* v);
        Node* init(SInterfaceBase* v);
        Node* init(SMethod* v);
        Node* init(SMethodParam* v);

        Node* init(SStruct* v);
        Node* init(SStructBase* v);
        Node* init(SStructField* v);

        Node* init(SException* v);
        Node* init(SExceptionBase* v);
        Node* init(SExceptionField* v);

        Node* init(SEnum* v);
        Node* init(SEnumField* v);

        Node* init(SFlags* v);
        Node* init(SFlagsField* v);

        Node* init(SAlias* v);

        Node* init(SScopedName* v);

        Node* init(STuple* v);
        Node* init(SList* v);
        Node* init(SSet* v);
        Node* init(SMap* v);
        Node* init(SPtr* v);
        Node* init(SOpt* v);
        Node* init(SArray* v);
        Node* init(SVariant* v);

        Node* init(SPrimitive* v);
    };
}
