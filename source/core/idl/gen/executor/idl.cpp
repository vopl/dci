/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "idl.hpp"

namespace dci::idl::gen::executor
{
    using namespace im::ast;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Idl::Idl()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Idl::~Idl()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Idl::sname()
    {
        return "idl";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Idl::name()
    {
        return sname();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Idl::description()
    {
        return "produce canonical idl representation";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    boost::program_options::options_description Idl::options()
    {
        auto local = Text::options();

        local.add_options()
            (
                (name()+"-write-signs").data(),
                "write sign values for general entities"
            )
            ;

        return local;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Idl::run(const im::Storage& ims, const boost::program_options::variables_map& vars)
    {
        if(!Text::run(ims, vars))
        {
            return false;
        }

        _writeSigns = vars.count(name()+"-write-signs") > 0;

        walk(ims.root());

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Primitive& v)
    {
        switch(v->kind)
        {
        case PrimitiveKind::none:
            _out<<"none";
            break;
        case PrimitiveKind::bool_:
            _out<<"bool";
            break;
        case PrimitiveKind::int8:
            _out<<"int8";
            break;
        case PrimitiveKind::int16:
            _out<<"int16";
            break;
        case PrimitiveKind::int32:
            _out<<"int32";
            break;
        case PrimitiveKind::int64:
            _out<<"int64";
            break;
        case PrimitiveKind::uint8:
            _out<<"uint8";
            break;
        case PrimitiveKind::uint16:
            _out<<"uint16";
            break;
        case PrimitiveKind::uint32:
            _out<<"uint32";
            break;
        case PrimitiveKind::uint64:
            _out<<"uint64";
            break;
        case PrimitiveKind::real32:
            _out<<"real32";
            break;
        case PrimitiveKind::real64:
            _out<<"real64";
            break;
        case PrimitiveKind::string:
            _out<<"string";
            break;
        case PrimitiveKind::bytes:
            _out<<"bytes";
            break;
        case PrimitiveKind::interface:
            _out<<"interface";
            break;
        case PrimitiveKind::iid:
            _out<<"iid";
            break;
        case PrimitiveKind::ilid:
            _out<<"ilid";
            break;
        case PrimitiveKind::exception:
            _out<<"exception";
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Array& v)
    {
        _out<<"array<";
        walk(v->elementType);
        _out<<", "<<v->size<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Tuple& v)
    {
        _out<<"tuple<";

        bool first = true;
        for(auto& t : v->elementTypes)
        {
            if(first) first = false;
            else _out<<", ";
            walk(t);
        }

        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Ptr& v)
    {
        _out<<"ptr<";
        walk(v->valueType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Opt& v)
    {
        _out<<"opt<";
        walk(v->valueType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const List& v)
    {
        _out<<"list<";
        walk(v->elementType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Map& v)
    {
        _out<<"map<";
        walk(v->keyType);
        _out<<", ";
        walk(v->valueType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Set& v)
    {
        _out<<"set<";
        walk(v->elementType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Alias& v)
    {
        _out<<el;

        writeSign(v);
        _out<<"alias "<<v->name->value<<" = ";
        walk(v->type);
        _out<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Enum& v)
    {
        _out<<el;

        writeSign(v);
        _out<<"enum "<<v->name->value<<el;
        auto _=_out.scope();
        Text::walk(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const EnumField& v)
    {
        _out<<el;

        writeSign(v);
        _out<<v->name->value<<" = "<<v->value<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Flags& v)
    {
        _out<<el;

        writeSign(v);
        _out<<"flags "<<v->name->value<<el;
        auto _=_out.scope();
        Text::walk(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const FlagsField& v)
    {
        _out<<el;

        writeSign(v);
        _out<<v->name->value<<" = "<<v->value<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Scope& v)
    {
        _out<<el;

        if(v->owner)
        {
            writeSign(v);
            _out<<"scope "<<v->name->value<<el;
            auto _=_out.scope();
            Text::walk(v);
        }
        else
        {
            //its global scope
            writeSign(v);
            Text::walk(v);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Variant& v)
    {
        _out<<"variant<";

        bool first = true;
        for(auto& t : v->elementTypes)
        {
            if(first) first = false;
            else _out<<", ";
            walk(t);
        }

        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Exception& v)
    {
        _out<<el;

        writeSign(v);
        _out<<"exception "<<v->name->value<<el;

        if(v->base)
        {
            _out<<indent;
            _out<<": ";
            walk(v->base);
            _out<<el;
            _out<<undent;
        }

        auto _=_out.scope();

        for(ExceptionField& field : v->fields)
        {
            walk(field);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const ExceptionBase& v)
    {
        _out<<v->scopedName->toString();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const ExceptionField& v)
    {
        _out<<el;

        writeSign(v);
        walk(v->type);
        _out<<" "<<v->name->value<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Struct& v)
    {
        _out<<el;

        writeSign(v);
        _out<<"struct "<<v->name->value<<el;

        _out<<indent;
        bool first = true;
        for(StructBase& base : v->bases)
        {
            _out<<(first ? ": " : ", ");
            walk(base);
            _out<<el;

            first = false;
        }
        _out<<undent;

        auto _=_out.scope();

        for(StructField& field : v->fields)
        {
            walk(field);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const StructBase& v)
    {
        _out<<v->scopedName->toString();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const StructField& v)
    {
        _out<<el;

        writeSign(v);
        walk(v->type);
        _out<<" "<<v->name->value<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Interface& v)
    {
        _out<<el;

        writeSign(v);
        _out<<"interface "<<v->name->value<<el;

        _out<<indent;
        bool first = true;
        for(InterfaceBase& base : v->bases)
        {
            _out<<(first ? ": " : ", ");
            walk(base);
            _out<<el;

            first = false;
        }
        _out<<undent;

        auto _=_out.scope();

        for(Method& method : v->methods)
        {
            walk(method);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const InterfaceBase& v)
    {
        _out<<v->scopedName->toString();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const Method& v)
    {
        _out<<el;

        writeSign(v);
        _out<< (MethodDirection::in == v->direction ? "in" : "out")<<" ";
        _out<<v->name->value<<": (";

        bool first = true;
        for(MethodParam& param : v->query)
        {
            if(first) first = false;
            else _out<<", ";

            walk(param);
        }
        _out<<")";

        if(!v->noreply)
        {
            _out<<" -> ";
            walk(v->reply);
        }
        _out<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const MethodParam& v)
    {
        walk(v->type);

        if(v->name && !v->name->value.empty())
        {
            _out<<" "<<v->name->value;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const TypeUse& v)
    {
        Text::walk(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Idl::walk(const im::ast::ScopedName& v)
    {
        _out<<v->toString();
    }

}

