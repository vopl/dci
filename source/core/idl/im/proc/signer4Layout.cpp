/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "signer4Layout.hpp"
#include "../signBuilder.hpp"
#include <dci/utils/dbg.hpp>
#include <dci/utils/b2h.hpp>

using namespace std::literals;

namespace dci::idl::im::proc
{
    using namespace ast;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Signer4Layout()
    {
    }

    namespace loop
    {
        std::map<Signer4Layout::Node*, std::size_t> loop;
        void init(std::map<Signer4Layout::Node*, std::size_t>& loop, Signer4Layout::Node* enter)
        {
            dbgAssert(!enter->_finalized);

            std::size_t& counter = loop[enter];
            if(counter < 2)
            {
                ++counter;
                for(const auto& dep : enter->_deps)
                    if(!dep._target->_finalized)
                        init(loop, dep._target);
            }
        }

        void crop(std::map<Signer4Layout::Node*, std::size_t>& loop)
        {
            for(auto iter{loop.begin()}; loop.end()!=iter; )
            {
                if(iter->second < 2)
                    iter = loop.erase(iter);
                else
                    ++iter;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Signer4Layout::exec(Scope& s)
    {
        init(s.get());

        std::set<Node*> nodes;
        for(auto& [k,n] : _nodes)
            nodes.emplace(&n);

        // toposort
        while(!nodes.empty())
        {
            // сначала отрабатываем независимые деревья
            std::size_t processed{};
            for(auto iter{nodes.begin()}; nodes.end()!=iter;)
            {
                Node& node = **iter;
                bool satisfied{true};
                for(const Dep& dep : node._deps)
                {
                    if(nodes.contains(dep._target))
                    {
                        satisfied = false;
                        break;
                    }
                }

                if(satisfied)
                {
                    finalize({&node});
                    iter = nodes.erase(iter);
                    ++processed;
                }
                else
                    ++iter;
            }

            if(!processed)
            {
                // остались только циклы и их ответвления, берем любой минимальный
                std::map<Node*, std::size_t> minLoop;
                for(Node* entry : nodes)
                {
                    if(minLoop.contains(entry))
                        continue;

                    std::map<Node*, std::size_t> loop;
                    loop::init(loop, entry);
                    loop::crop(loop);
                    dbgAssert(!loop.empty());

                    if(minLoop.empty() || minLoop.size() > loop.size())
                        minLoop = std::move(loop);
                }
                dbgAssert(!minLoop.empty());

                std::deque<Node*> stableMinLoop;
                for(auto& [node, _] : minLoop)
                    stableMinLoop.emplace_back(node);
                std::sort(stableMinLoop.begin(), stableMinLoop.end(), [](Node* a, Node* b){ return a->_rise < b->_rise; });

                finalize(stableMinLoop);

                for(Node* node : stableMinLoop)
                    nodes.erase(node);
                processed += stableMinLoop.size();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class AstNode>
    bool Signer4Layout::emplace(AstNode* astNode, Node*& node)
    {
        auto eres = _nodes.emplace(std::piecewise_construct, std::tie(astNode), std::tuple{});
        node = &eres.first->second;
        if(!eres.second)
            return false;

        node->_result = &astNode->sign4Layout;
        dbgAssert(*node->_result == Sign{});

        {
            SignBuilder sb;
            sb.add(nodeTag<AstNode>);
            node->_rise = sb.finish();
        }
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Signer4Layout::finalize(const std::deque<Node*>& loop)
    {
        auto addNode = [&](SignBuilder& sb, Node& node)
        {
            sb.add("node"sv);
            sb.add(node._rise);

            sb.add("deps"sv);
            sb.add(static_cast<uint32_t>(node._deps.size()));
            for(std::size_t i{}; i<node._deps.size(); ++i)
            {
                sb.add("dep"sv);
                sb.add(static_cast<uint32_t>(i));

                const Dep& dep = node._deps[i];
#ifndef NDEBUG
                bool inLoop = loop.end()!=std::find(loop.begin(), loop.end(), dep._target);
                dbgAssert(dep._target->_finalized || inLoop);
#endif

                sb.add("kind"sv);
                sb.add(dep._kind);

                sb.add("index"sv);
                sb.add(static_cast<uint32_t>(dep._index));

                sb.add("value"sv);
                sb.add(dep._target->_rise);
            }
        };

        SignBuilder sb;
        {
            sb.add("loop"sv);
            sb.add(static_cast<uint32_t>(loop.size()));
            for(std::size_t i{}; i<loop.size(); ++i)
            {
                sb.add("index"sv);
                sb.add(static_cast<uint32_t>(i));

                Node& node = *loop[i];
                dbgAssert(!node._finalized);
                addNode(sb, node);
            }
        }

        for(std::size_t i{}; i<loop.size(); ++i)
        {
            SignBuilder sbi{sb};
            sbi.add("self"sv);
            sbi.add(static_cast<uint32_t>(i));

            Node& node = *loop[i];
            node._rise = sbi.finish();
            dbgAssert(!node._finalized);
            dbgAssert(*node._result == Sign{});
            *node._result = node._rise;
            node._finalized = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Part>
    void Signer4Layout::addPart(Node* node, std::string_view tag, const Part& part)
    {
        return addPart(node, tag, std::size_t{}, part);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Part>
    void Signer4Layout::addPart(Node* node, std::string_view tag, std::size_t idx, const std::shared_ptr<Part>& part)
    {
        return addPart(node, tag, idx, part.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Part>
    void Signer4Layout::addPart(Node* node, std::string_view tag, std::size_t idx, const Part& part)
    {
        if constexpr(std::is_enum_v<Part>)
            addPart(node, tag, std::to_underlying(part));
        else if constexpr(std::is_integral_v<Part> || std::is_same_v<Part, std::string>)
        {
            SignBuilder sb;
            sb.add(node->_rise);
            sb.add(tag);
            sb.add(idx);
            sb.add("part"sv);
            sb.add(part);
            node->_rise = sb.finish();
        }
        else if constexpr(requires{part.size(); part.begin(); part.end();})
        {
            for(std::size_t i{}; i<part.size(); ++i)
                addPart(node, tag, idx*part.size() + i, part[i]);
        }
        else if constexpr(requires{part.which();})
        {
            boost::apply_visitor([&](const auto& element)
            {
                constexpr auto vsize = boost::mpl::size<typename Part::types>::type::value;
                addPart(node, tag, idx*vsize + part.which(), element);
            }, part);
        }
        else  if constexpr(std::is_pointer_v<Part>)
        {
            if(part)
                node->_deps.emplace_back(Dep{std::string{tag}, idx, init(part)});
        }
        else
            static_assert(false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SScope* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            for(auto e : v->aliases      ) init(e);
            for(auto e : v->structs      ) init(e);
            for(auto e : v->enums        ) init(e);
            for(auto e : v->flagses      ) init(e);
            for(auto e : v->exceptions   ) init(e);
            for(auto e : v->interfaces   ) init(e);
            for(auto e : v->interfaces   ) init(e->opposite);
            for(auto e : v->scopes       ) init(e);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SInterface* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "isPrimary"sv, v->isPrimary);
            addPart(node, "bases"sv, v->bases);
            addPart(node, "methods"sv, v->methods);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SInterfaceBase* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "instance"sv, v->instance);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SMethod* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "direction"sv, v->direction);
            addPart(node, "query"sv, v->query);
            addPart(node, "noreply"sv, v->noreply);
            addPart(node, "reply"sv, v->reply);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SMethodParam* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "type"sv, v->type);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SStruct* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "type"sv, v->bases);
            addPart(node, "fields"sv, v->fields);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SStructBase* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "instance"sv, v->instance);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SStructField* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "type"sv, v->type);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SException* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "base"sv, v->base);
            addPart(node, "fields"sv, v->fields);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SExceptionBase* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "instance"sv, v->instance);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SExceptionField* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "type"sv, v->type);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SEnum* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "fields"sv, v->fields);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SEnumField* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "value"sv, v->value);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SFlags* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "fields"sv, v->fields);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SFlagsField* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "value"sv, v->value);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SAlias* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "type"sv, v->type);
        }
        return node;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Layout::Node* Signer4Layout::init(SScopedName* v)
    {
        return boost::apply_visitor([&](const auto& decl)
        {
            dbgAssert(decl);
            return init(decl);
        }, v->asDecl);
    }

    Signer4Layout::Node* Signer4Layout::init(STuple* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "elementTypes", v->elementTypes);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SList* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "elementType", v->elementType);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SSet* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "elementType", v->elementType);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SMap* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "keyType", v->keyType);
            addPart(node, "valueType", v->valueType);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SPtr* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "valueType", v->valueType);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SOpt* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "valueType", v->valueType);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SArray* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "elementType", v->elementType);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SVariant* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "elementTypes", v->elementTypes);
        }
        return node;
    }

    Signer4Layout::Node* Signer4Layout::init(SPrimitive* v)
    {
        Node* node;
        if(emplace(v, node))
        {
            addPart(node, "elementTypes", v->kind);
        }
        return node;
    }
}
