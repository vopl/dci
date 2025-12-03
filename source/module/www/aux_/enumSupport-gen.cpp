// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "enumSupport.hpp"
#include "www.hpp"
#include <iostream>
#include <iomanip>
#include <regex>
#include <dci/utils/integer.hpp>

using namespace dci::idl;
using namespace dci::idl::gen::www::http;
using namespace dci::module::www::enumSupport;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <class E, std::size_t FI = 0>
void enumFields(auto f)
{
    if constexpr(FI < introspection::fieldsCount<E>)
    {
        f(introspection::fieldName<E, FI>, introspection::fieldValue<E, FI>);
        enumFields<E, FI+1>(f);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <class E>
std::string adaptFieldNameString(std::string src)
{
    return src;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <>
std::string adaptFieldNameString<firstLine::Version>(std::string str)
{
    str = std::regex_replace(str, std::regex{"HTTP_"}, "HTTP/");
    str = std::regex_replace(str, std::regex{"_"}, ".");
    return str;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <>
std::string adaptFieldNameString<header::KeyRecognized>(std::string str)
{
    str = std::regex_replace(str, std::regex{"_"}, "-");
    return str;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
std::string prf(std::size_t indent)
{
    return std::string(indent*4, ' ');
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <class E>
void write_toString(std::size_t indent)
{
    std::string nameStr{introspection::typeName<E>.data(), introspection::typeName<E>.size()-1};

    std::cout << prf(indent) << "/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7" << std::endl;
    std::cout << prf(indent) << "template <> std::optional<std::string_view> toString<" << nameStr << ">(" << nameStr << " e)" << std::endl;
    std::cout << prf(indent) << "{" << std::endl;
    ++indent;
    std::cout << prf(indent) << "using namespace std::literals::string_view_literals;" << std::endl;
    std::cout << prf(indent) << "switch(e)" << std::endl;
    std::cout << prf(indent) << "{" << std::endl;

    enumFields<E>([&](auto name, auto value)
    {
        if(E{} == value)
            return;

        std::string fieldNameStr{name.data(), name.size()-1};
        std::cout << prf(indent) << "case " << nameStr << "::" << fieldNameStr << ": return \"" << adaptFieldNameString<E>(fieldNameStr) << "\"sv;" << std::endl;
    });

    std::cout << prf(indent) << "default: break;" << std::endl;
    std::cout << prf(indent) << "}" << std::endl;
    std::cout << prf(indent) << "return {};" << std::endl;
    --indent;
    std::cout << prf(indent) << "}" << std::endl;
    std::cout << std::endl;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
std::string escapedString(std::string_view str)
{
    std::string res;

    res += '"';
    for(unsigned char c : str)
    {
        switch(c)
        {
        case '\'':  res += "\\'";  break;
        case '\"':  res += "\\\"";  break;
        case '\?':  res += "\\\?";  break;
        case '\\':  res += "\\\\";  break;
        case '\a':  res += "\\\a";  break;
        case '\b':  res += "\\\b";  break;
        case '\f':  res += "\\\f";  break;
        case '\n':  res += "\\\n";  break;
        case '\r':  res += "\\\r";  break;
        case '\t':  res += "\\\t";  break;
        case '\v':  res += "\\\v";  break;
        default:
            if(isprint(c))
                res += c;
            else
            {
                res.resize(res.size() + 6);
                std::sprintf(res.data()+res.size()-6, "\\x{%02x}", (unsigned)c);
            }
        }
    }
    res += '"';

    return res;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <std::size_t size>
std::string escapedString(const std::array<char, size>& str)
{
    return escapedString(std::string_view{str.data(), str.size()});
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <class Value, bool lowerCase = false>
struct Trie
{
    template <std::size_t keySize>
    using Key = std::array<char, keySize>;

    std::map<Key<0>, Value> leafs0;
    std::map<Key<1>, Value> leafs1;
    std::map<Key<2>, Value> leafs2;
    std::map<Key<3>, Value> leafs3;
    std::map<Key<4>, Value> leafs4;
    std::map<Key<5>, Value> leafs5;
    std::map<Key<6>, Value> leafs6;
    std::map<Key<7>, Value> leafs7;
    std::map<Key<8>, Trie<Value, lowerCase>> nodes;

    template <std::size_t keySize>
    static std::string str4CaseLabel(const Key<keySize>& key)
    {
        std::stringstream out;
        out << "0x" << std::hex << std::setfill('0') << std::setw(keySize*2) << static_cast<std::uint64_t>(asKey<keySize, lowerCase>(key.data()));
        if(keySize > 4)
            out << "ull";
        return out.str();
    }

    template <std::size_t keySize>
    Key<keySize> unshiftKey(std::string_view& path)
    {
        if(path.size() < keySize)
            throw "never";

        Key<keySize> key;
        std::copy(path.begin(), path.begin()+keySize, key.begin());
        path.remove_prefix(keySize);
        return key;
    }

    void push(std::string_view path, Value value)
    {
        switch(path.size())
        {
        case 0: leafs0.emplace(unshiftKey<0>(path), value); return;
        case 1: leafs1.emplace(unshiftKey<1>(path), value); return;
        case 2: leafs2.emplace(unshiftKey<2>(path), value); return;
        case 3: leafs3.emplace(unshiftKey<3>(path), value); return;
        case 4: leafs4.emplace(unshiftKey<4>(path), value); return;
        case 5: leafs5.emplace(unshiftKey<5>(path), value); return;
        case 6: leafs6.emplace(unshiftKey<6>(path), value); return;
        case 7: leafs7.emplace(unshiftKey<7>(path), value); return;
        default: nodes[unshiftKey<8>(path)].push(path, value); return;
        }
    }

    void collectSizes(std::set<std::size_t>& sizes, std::size_t headSize=0) const
    {
        if(!leafs0.empty()) sizes.emplace(headSize+0);
        if(!leafs1.empty()) sizes.emplace(headSize+1);
        if(!leafs2.empty()) sizes.emplace(headSize+2);
        if(!leafs3.empty()) sizes.emplace(headSize+3);
        if(!leafs4.empty()) sizes.emplace(headSize+4);
        if(!leafs5.empty()) sizes.emplace(headSize+5);
        if(!leafs6.empty()) sizes.emplace(headSize+6);
        if(!leafs7.empty()) sizes.emplace(headSize+7);
        for(const auto&[_, node] : nodes)
            node.collectSizes(sizes, headSize+8);
    }

    bool hasTailSize(std::size_t tailSize) const
    {
        switch(tailSize)
        {
        case 0: return !leafs0.empty();
        case 1: return !leafs1.empty();
        case 2: return !leafs2.empty();
        case 3: return !leafs3.empty();
        case 4: return !leafs4.empty();
        case 5: return !leafs5.empty();
        case 6: return !leafs6.empty();
        case 7: return !leafs7.empty();
        default:
            for(const auto&[_, node] : nodes)
                if(node.hasTailSize(tailSize-8))
                    return true;
        }

        return false;
    }

    void generateCpp_sizeThenKey(std::size_t indent) const
    {
        std::set<std::size_t> sizes;
        collectSizes(sizes);

        std::cout << prf(indent) << "switch(size)\n";
        std::cout << prf(indent) << "{\n";
        for(std::size_t size : sizes)
        {
            std::cout << prf(indent) << "case " << size << ":\n";
            generateCpp_sizeThenKey(indent+1, 0, size);
        }
        std::cout << prf(indent) << "}\n";
        std::cout << prf(indent) << "return {};\n";
    }

    void generateCpp_sizeThenKey(std::size_t indent, std::size_t headSize, std::size_t tailSize) const
    {
        switch(tailSize)
        {
        case 0: if(!leafs0.empty()) generateCpp_sizeThenKey(indent, leafs0, headSize); return;
        case 1: if(!leafs1.empty()) generateCpp_sizeThenKey(indent, leafs1, headSize); return;
        case 2: if(!leafs2.empty()) generateCpp_sizeThenKey(indent, leafs2, headSize); return;
        case 3: if(!leafs3.empty()) generateCpp_sizeThenKey(indent, leafs3, headSize); return;
        case 4: if(!leafs4.empty()) generateCpp_sizeThenKey(indent, leafs4, headSize); return;
        case 5: if(!leafs5.empty()) generateCpp_sizeThenKey(indent, leafs5, headSize); return;
        case 6: if(!leafs6.empty()) generateCpp_sizeThenKey(indent, leafs6, headSize); return;
        case 7: if(!leafs7.empty()) generateCpp_sizeThenKey(indent, leafs7, headSize); return;
        default: if(hasTailSize(tailSize)) generateCpp_sizeThenKey(indent, nodes, headSize, tailSize); return;
        }
    }

    template <std::size_t keySize>
    static void generateCpp_sizeThenKey(std::size_t indent, const std::map<Key<keySize>, Value>& leafs, std::size_t headSize)
    {
        if(!keySize)
        {
            //assert(1 == leafs.size());
            std::cout << prf(indent) << "return " << leafs.begin()->second << ";\n";
        }
        else
        {
            std::cout << prf(indent) << "switch(asKey<" << keySize << "" << (lowerCase ? ", true" : "") << ">(data+" << headSize << "))\n";
            std::cout << prf(indent) << "{\n";
            for(const auto&[key, value] : leafs)
                std::cout << prf(indent) << "case " << str4CaseLabel(key) << ": return " << value << "; // " << escapedString(key) << "\n";
            std::cout << prf(indent) << "default: return {};\n";
            std::cout << prf(indent) << "}\n";
        }
    }

    static void generateCpp_sizeThenKey(std::size_t indent, const std::map<Key<8>, Trie<Value, lowerCase>>& nodes, std::size_t headSize, std::size_t tailSize)
    {
        std::cout << prf(indent) << "switch(asKey<8" << (lowerCase ? ", true" : "") << ">(data+" << headSize << "))\n";
        std::cout << prf(indent) << "{\n";
        for(const auto&[key, node] : nodes)
        {
            if(node.hasTailSize(tailSize-8))
            {
                std::cout << prf(indent) << "case " << str4CaseLabel(key) << ": // " << escapedString(key) << "\n";
                node.generateCpp_sizeThenKey(indent+1, headSize+8, tailSize-8);
            }
        }
        std::cout << prf(indent) << "default: return {};\n";
        std::cout << prf(indent) << "}\n";
    }

    void generateCpp_keyThenSize(std::size_t indent) const
    {
        if( leafs0.empty() &&
                leafs1.empty() &&
                leafs2.empty() &&
                leafs3.empty() &&
                leafs4.empty() &&
                leafs5.empty() &&
                leafs6.empty() &&
                leafs7.empty() &&
                nodes.empty())
        {
            std::cout << prf(indent) << "return {};\n";
        }
        else
        {
            std::cout << prf(indent) << "switch(size)\n";
            std::cout << prf(indent) << "{\n";
            std::cout << prf(indent) << "case 0:"; generateCpp_keyThenSize(indent+1, leafs0);
            std::cout << prf(indent) << "case 1:"; generateCpp_keyThenSize(indent+1, leafs1);
            std::cout << prf(indent) << "case 2:"; generateCpp_keyThenSize(indent+1, leafs2);
            std::cout << prf(indent) << "case 3:"; generateCpp_keyThenSize(indent+1, leafs3);
            std::cout << prf(indent) << "case 4:"; generateCpp_keyThenSize(indent+1, leafs4);
            std::cout << prf(indent) << "case 5:"; generateCpp_keyThenSize(indent+1, leafs5);
            std::cout << prf(indent) << "case 6:"; generateCpp_keyThenSize(indent+1, leafs6);
            std::cout << prf(indent) << "case 7:"; generateCpp_keyThenSize(indent+1, leafs7);
            std::cout << prf(indent) << "default:";  generateCpp_keyThenSize(indent+1, nodes);

            std::cout << prf(indent) << "}\n";
        }
    }

    template <std::size_t keySize>
    static void generateCpp_keyThenSize(std::size_t indent, const std::map<Key<keySize>, Value>& leafs)
    {
        if(leafs.empty())
        {
            std::cout << " return {};\n";
        }
        else
        {
            if(!keySize)
            {
                //assert(1 == leafs.size());
                std::cout << " return " << leafs.begin()->second << ";\n";
            }
            else
            {
                std::cout << "\n";
                std::cout << prf(indent) << "switch(asKey<" << keySize << "" << (lowerCase ? ", true" : "") << ">(data))\n";
                std::cout << prf(indent) << "{\n";
                for(const auto&[key, value] : leafs)
                    std::cout << prf(indent) << "case " << str4CaseLabel(key) << ": return " << value << "; // " << escapedString(key) << "\n";
                std::cout << prf(indent) << "default: return {};\n";
                std::cout << prf(indent) << "}\n";
            }
        }
    }

    static void generateCpp_keyThenSize(std::size_t indent, const std::map<Key<8>, Trie<Value, lowerCase>>& nodes)
    {
        if(nodes.empty())
        {
            std::cout << " return {};\n";
        }
        else
        {
            std::cout << "\n";
            std::cout << prf(indent) << "switch(asKey<8" << (lowerCase ? ", true" : "") << ">(data))\n";
            std::cout << prf(indent) << "{\n";
            for(const auto&[key, node] : nodes)
            {
                std::cout << prf(indent) << "case " << str4CaseLabel(key) << ": data += 8; size -= 8; // " << escapedString(key) << "\n";
                node.generateCpp_keyThenSize(indent+1);
            }
            std::cout << prf(indent) << "default: return {};\n";
            std::cout << prf(indent) << "}\n";
        }
    }
};

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
template <class E, bool lowerCase>
void write_toEnum(std::size_t indent)
{
    std::string nameStr{introspection::typeName<E>.data(), introspection::typeName<E>.size()-1};

    std::cout << prf(indent) << "/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7" << std::endl;
    std::cout << prf(indent) << "template <> std::optional<" << nameStr << "> toEnum<" << nameStr << ">(std::string_view s)" << std::endl;
    std::cout << prf(indent) << "{" << std::endl;
    ++indent;

    std::cout << prf(indent) << "std::size_t size = s.size();" << std::endl;
    std::cout << prf(indent) << "const char* data = s.data();" << std::endl;

    Trie<std::string, lowerCase> trie;

    enumFields<E>([&](auto name, auto value)
    {
        if(E{} == value)
            return;

        std::string fieldNameStr{name.data(), name.size()-1};
        trie.push(adaptFieldNameString<E>(fieldNameStr), nameStr + "::" + fieldNameStr);
    });

    trie.generateCpp_sizeThenKey(indent);

    --indent;
    std::cout << prf(indent) << "}" << std::endl;
    std::cout << std::endl;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
int main(/*int c_argc, char* c_argv[]*/)
{
    std::cout << "#include \"pch.hpp\"" << std::endl;
    std::cout << "#include \"enumSupport.hpp\"" << std::endl;
    std::cout << std::endl;
    std::cout << "namespace dci::module::www::enumSupport" << std::endl;
    std::cout << "{" << std::endl;

    write_toString<firstLine::Method>(1);
    write_toString<firstLine::Version>(1);
    write_toString<message::tunable::Compression>(1);
    write_toString<header::KeyRecognized>(1);

    write_toEnum<firstLine::Method, false>(1);
    write_toEnum<firstLine::Version, false>(1);
    write_toEnum<message::tunable::Compression, false>(1);
    write_toEnum<header::KeyRecognized, true>(1);

    std::cout << "}" << std::endl;
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
