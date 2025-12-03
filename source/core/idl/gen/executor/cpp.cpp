// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "cpp.hpp"

namespace dci::idl::gen::executor
{
    using namespace im::ast;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cpp::Cpp()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cpp::~Cpp()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::sname()
    {
        return "cpp";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::name()
    {
        return sname();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::description()
    {
        return "produce common c++ realization";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    boost::program_options::options_description Cpp::options()
    {
        auto local = Text::options();

        local.add_options()
                (
                    (name()+"-no-entities").data(),
                    "does not generate entities"
                )
                (
                    (name()+"-stiac-support").data(),
                    "generate stiac support (links)"
                )
                (
                    (name()+"-stiac-support-filter").data(),
                    boost::program_options::value<std::string>()->default_value(".*"),
                    "scope name regexp for stiac support generation"
                )
                ;

        return local;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Cpp::run(const im::Storage& ims, const boost::program_options::variables_map& vars)
    {
        if(!Text::run(ims, vars))
        {
            return false;
        }

        _writeEntities = !vars.count(name()+"-no-entities");

        _writeStiacSupport = vars.count(name()+"-stiac-support");
        _stiacSupportFilter = std::regex(vars[name()+"-stiac-support-filter"].as<std::string>());

        _out<<"// dci-idl to cpp"<<el;
        _out<<"#pragma once"<<el;

        if(_writeEntities)
        {
            _out<<"#include <dci/idl.hpp>"<<el;
            _out<<"#include <dci/bytes.hpp>"<<el;
            _out<<"#include <dci/sbs/wire.hpp>"<<el;
            _out<<"#include <dci/idl/flagsSupport.hpp>"<<el;

            {
                _out<<el;
                _out<<"namespace dci::idl::gen"<<el;
                auto _=_out.scope();

                walk(ims.root());
            }

            {
                _out<<el;
                _out<<"namespace dci::idl::contract"<<el;
                auto _=_out.scope();

                writeBundles(ims.root());
                writeBundleContainers(ims.root());
                writeHmDescriptors(ims.root());
                writeMdDescriptors(ims.root());
                writeHmDescriptorMethods(ims.root());
                writeMdDescriptorMethods(ims.root());
            }

            {
                _out<<"namespace dci::idl::gen"<<el;
                auto _=_out.scope();

                writeInterfaceMethodBodies(ims.root());
            }

            {
                _out<<"namespace dci::idl::interface"<<el;
                auto _=_out.scope();

                writeMethods(ims.root());
            }

            {
                _out<<el;
                _out<<"#include <dci/idl/introspection.hpp>"<<el;
                _out<<"namespace dci::idl::introspection"<<el;
                {
                    auto _=_out.scope();
                    writeIntrospection(ims.root());
                }
            }

            {
                _out<<"namespace dci::idl::gen"<<el;
                auto _=_out.scope();

                writeStructAndExceptionOperators(ims.root());
            }
        }

        if(_writeStiacSupport)
        {
            _out<<"#include <dci/stiac.hpp>"<<el;

            _out<<el;

            _out<<"namespace dci::stiac::link"<<el;
            {
                auto _=_out.scope();
                writeStiacLinks(ims.root());
                writeStiacLinkMethodBodies(ims.root());
            }
            _out<<"namespace dci::stiac"<<el;
            {
                auto _=_out.scope();
                writeStiacExceptionSerializers(ims.root());
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Primitive& v)
    {
        switch(v->kind)
        {
        case PrimitiveKind::none:
            _out<<nameInPrimitives("None");
            break;
        case PrimitiveKind::bool_:
            _out<<nameInPrimitives("bool_");
            break;
        case PrimitiveKind::int8:
            _out<<nameInPrimitives("int8");
            break;
        case PrimitiveKind::int16:
            _out<<nameInPrimitives("int16");
            break;
        case PrimitiveKind::int32:
            _out<<nameInPrimitives("int32");
            break;
        case PrimitiveKind::int64:
            _out<<nameInPrimitives("int64");
            break;
        case PrimitiveKind::uint8:
            _out<<nameInPrimitives("uint8");
            break;
        case PrimitiveKind::uint16:
            _out<<nameInPrimitives("uint16");
            break;
        case PrimitiveKind::uint32:
            _out<<nameInPrimitives("uint32");
            break;
        case PrimitiveKind::uint64:
            _out<<nameInPrimitives("uint64");
            break;
        case PrimitiveKind::real32:
            _out<<nameInPrimitives("real32");
            break;
        case PrimitiveKind::real64:
            _out<<nameInPrimitives("real64");
            break;
        case PrimitiveKind::string:
            _out<<nameInPrimitives("String");
            break;
        case PrimitiveKind::bytes:
            _out<<nameInDci("Bytes");
            break;
        case PrimitiveKind::interface:
            _out<<nameInRoot("Interface");
            break;
        case PrimitiveKind::iid:
            _out<<nameInSparesI("Id");
            break;
        case PrimitiveKind::ilid:
            _out<<nameInSparesI("Lid");
            break;
        case PrimitiveKind::exception:
            _out<<nameInPrimitives("ExceptionPtr");
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Array& v)
    {
        _out<<nameInPrimitives("Array")<<"<";
        walk(v->elementType);
        _out<<", "<<v->size<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Tuple& v)
    {
        _out<<nameInPrimitives("Tuple")<<"<";

        {
            auto _=_out.scope("", "");

            bool first = true;
            for(auto& t : v->elementTypes)
            {
                if(first) first = false;
                else _out<<",";
                walk(t);
            }
        }

        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Ptr& v)
    {
        _out<<nameInPrimitives("Ptr")<<"<";
        walk(v->valueType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Opt& v)
    {
        _out<<nameInPrimitives("Opt")<<"<";
        walk(v->valueType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const List& v)
    {
        _out<<nameInPrimitives("List")<<"<";
        walk(v->elementType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Map& v)
    {
        _out<<nameInPrimitives("Map")<<"<";
        walk(v->keyType);
        _out<<", ";
        walk(v->valueType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Set& v)
    {
        _out<<nameInPrimitives("Set")<<"<";
        walk(v->elementType);
        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Alias& v)
    {
        _out<<el;
        _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
        _out<<"//sign: "<<((v->sign4Name ^ v->sign4Layout).toHex())<<" = "<<v->sign4Name.toHex()<<" ^ "<<v->sign4Layout.toHex()<<el;
        _out<<"using "<<v->name->value<<" = ";
        walk(v->type);
        _out<<";"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Enum& v)
    {
        _out<<el;
        _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
        _out<<"//sign: "<<((v->sign4Name ^ v->sign4Layout).toHex())<<" = "<<v->sign4Name.toHex()<<" ^ "<<v->sign4Layout.toHex()<<el;
        _out<<"//enum "<<v->name->value<<el;
        _out<<"enum class "<<v->name->value<<" : "<<nameInPrimitives("uint64")<<el;
        {
            auto _=_out.scope("{", "};");
            Text::walk(v);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const EnumField& v)
    {
        _out<<v->name->value;
        if(!v->value.empty())
        {
            _out<<" = "<<nameInPrimitives("uint64")<<"("<<v->value<<")";
        }
        _out<<","<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Flags& v)
    {
        _out<<el;
        _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
        _out<<"//sign: "<<((v->sign4Name ^ v->sign4Layout).toHex())<<" = "<<v->sign4Name.toHex()<<" ^ "<<v->sign4Layout.toHex()<<el;
        _out<<"//flags "<<v->name->value<<el;
        _out<<"enum class "<<v->name->value<<" : "<<nameInPrimitives("uint64")<<el;
        {
            auto _=_out.scope("{", "};");
            Text::walk(v);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const FlagsField& v)
    {
        _out<<v->name->value;
        if(!v->value.empty())
        {
            _out<<" = "<<nameInPrimitives("uint64")<<"("<<v->value<<")";
        }
        _out<<","<<el;
    }
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Scope& v)
    {
        if(v->owner)
        {
            _out<<"namespace "<<v->name->value<<el;
            auto _=_out.scope();
            Text::walk(v);

            if(!v->flagses.empty()) writeFlagsSupport();
        }
        else
        {
            //its global scope
            Text::walk(v);

            if(!v->flagses.empty()) writeFlagsSupport();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Variant& v)
    {
        _out<<nameInPrimitives("Variant")<<"<";

        {
            auto _=_out.scope("", "");

            bool first = true;
            for(auto& t : v->elementTypes)
            {
                if(first) first = false;
                else _out<<",";
                _out<<el;
                walk(t);
            }
            _out<<el;
        }

        _out<<">";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Exception& v)
    {
        _out<<el;
        _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
        _out<<"//sign: "<<((v->sign4Name ^ v->sign4Layout).toHex())<<" = "<<v->sign4Name.toHex()<<" ^ "<<v->sign4Layout.toHex()<<el;
        _out<<"//exception "<<v->name->value<<el;
        _out<<"struct "<<v->name->value<<el;

        _out<<indent;

        std::string mainBase;
        if(v->base)
        {
            mainBase = nameInGen(v->base->scopedName->toFullScopedName()->toString());
        }
        else
        {
            mainBase = nameInDci("Exception");
        }

        std::string skeleton = nameInDci("exception::Skeleton")+"<"+v->name->value+", "+mainBase+">";

        _out<<": public "<<skeleton<<el;
        _out<<undent;

        auto _=_out.scope("{", "};");

        _out<<"using "<<skeleton<<"::Skeleton;"<<el;

        for(ExceptionField& field : v->fields)
        {
            walk(field);
        }

        _out<<"std::partial_ordering operator<=>(const "<<v->name->value<<"& rhs) const;"<<el;
        _out<<"bool operator==(const "<<v->name->value<<"& rhs) const;"<<el;

        _out<<undent;
        _out<<"public:"<<el;
        _out<<indent;

        _out<<"static constexpr "<<nameInDci("Eid")<<" _eid {"<<idInitializer((v->sign4Name ^ v->sign4Layout))<<"};"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const ExceptionBase& v)
    {
        walk(v->scopedName);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const ExceptionField& v)
    {
        walk(v->type);
        _out<<" "<<v->name->value<<" {};"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Struct& v)
    {
        _out<<el;
        _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
        _out<<"//sign: "<<((v->sign4Name ^ v->sign4Layout).toHex())<<" = "<<v->sign4Name.toHex()<<" ^ "<<v->sign4Layout.toHex()<<el;
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

        auto _=_out.scope("{", "};");

        for(StructField& field : v->fields)
        {
            walk(field);
        }

        _out<<"std::partial_ordering operator<=>(const "<<v->name->value<<"& rhs) const;"<<el;
        _out<<"bool operator==(const "<<v->name->value<<"& rhs) const;"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const StructBase& v)
    {
        walk(v->scopedName);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const StructField& v)
    {
        walk(v->type);
        _out<<" "<<v->name->value<<" {};"<<el;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const Interface& v)
    {
        auto writer = [&](SInterface* v)
        {
            _out<<"//sign: "<<((v->sign4Name ^ v->sign4Layout).toHex())<<" = "<<v->sign4Name.toHex()<<" ^ "<<v->sign4Layout.toHex()<<el;

            const std::string sname = nameInSparesI(v->isPrimary?"Side::primary":"Side::opposite");
            const std::string bname = nameInSparesI("ImplBase")+"<"+v->name->value+", "+sname+">";
            _out<<"template <>"<<el;
            _out<<"struct "<<v->name->value<<"<"<<sname<<">"<<el;

            {
                auto _=_out.scope("", "");

                _out<<": public "<<bname<<el;
            }

            auto _=_out.scope("{", "};");

            _out<<"using "<<bname<<"::ImplBase;"<<el;
            _out<<v->name->value<<"(const "<<v->name->value<<"&);"<<el;
            _out<<v->name->value<<"("<<v->name->value<<"&&);"<<el;
            _out<<"using "<<bname<<"::operator=;"<<el;
            _out<<v->name->value<<"& operator=(const "<<v->name->value<<"&);"<<el;
            _out<<v->name->value<<"& operator=("<<v->name->value<<"&&);"<<el;
            _out<<"using "<<bname<<"::operator==;"<<el;
            _out<<"using "<<bname<<"::operator!=;"<<el;
            _out<<"using "<<bname<<"::operator<=>;"<<el;
        };

        _out<<el;
        _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
        _out<<"//interface "<<v->name->value<<el;
        _out<<"template <"<<nameInSparesI("Side")<<" side = "<<nameInSparesI("Side::primary")<<"> struct "<<v->name->value<<";"<<el;
        writer(v.get());

        _out<<"//opposite for "<<v->name->value<<el;
        writer(v->opposite);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//    void Cpp::walk(const InterfaceBase& v)
//    {
//        walk(v->scopedName);
//    }

//    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//    void Cpp::walk(const Method& v)
//    {
//    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//    void Cpp::walk(const MethodParam& v)
//    {
//        walk(v->type);

//        if(v->name && !v->name->value.empty())
//        {
//            _out<<" "<<v->name->value;
//        }
//    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const TypeUse& v)
    {
        Text::walk(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::walk(const ScopedName& v)
    {
        auto fullScopedName = v->toFullScopedName();
        fullScopedName->root = false;
        _out<<nameInGen(fullScopedName->toString());

        SInterface** interface = boost::get<SInterface*>(&v->asDecl);
        if(interface)
        {
            _out<<"<"<<nameInSparesI((*interface)->isPrimary ? "Side::primary" : "Side::opposite")<<">";
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        void enumerateInterfaces(const SScope* s, const auto& functor)
        {
            for(const auto& s : s->scopes) enumerateInterfaces(s, functor);
            for(const auto& i : s->interfaces) functor(i);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeFlagsSupport()
    {
        auto op = [&](const char* name)
        {
            _out<<"using "<<nameInRoot("flagsSupport")<<"::operator"<<name<<";"<<el;
        };

        op("<=>");

        op("!");
        op("&&");
        op("||");

        op("~");
        op("&");
        op("|");
        op("^");
        op(">>");
        op("<<");

        op("&=");
        op("|=");
        op("^=");
        op(">>=");
        op("<<=");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeIntrospection(const ast::Scope& v)
    {
        auto processorCommon = [&](const auto* v)
        {
            std::string name;
            {
                _out.push();
                walk(v->prepareFullScopedName());
                name = _out.pop(false);
            }

            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//introspection for "<<name<<el;

            return name;
        };

        auto processorFlags = [&](const std::string& name, const SFlags* v)
        {
            _out<<"template <> inline constexpr "<<nameInIntrospection("Kind")<<" kind<"<<name<<"> = "<<nameInIntrospection("Kind")<<"::flags;"<<el;
            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" fieldsCount<"<<name<<"> = "<<v->fields.size()<<";"<<el;

            for(std::size_t idx(0); idx<v->fields.size(); ++idx)
            {
                std::string fname = v->fields[idx]->name->value;
                _out<<"template <> inline constexpr std::array fieldName<"<<name<<", "<<idx<<"> = std::to_array(\""<<fname<<"\");"<<el;
                _out<<"template <> inline constexpr "<<name<<" fieldValue<"<<name<<", "<<idx<<"> = "<<name<<"::"<<fname<<";"<<el;
            }
        };

        auto processorEnum = [&](const std::string& name, const SEnum* v)
        {
            _out<<"template <> inline constexpr "<<nameInIntrospection("Kind")<<" kind<"<<name<<"> = "<<nameInIntrospection("Kind")<<"::enum_;"<<el;
            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" fieldsCount<"<<name<<"> = "<<v->fields.size()<<";"<<el;

            for(std::size_t idx(0); idx<v->fields.size(); ++idx)
            {
                std::string fname = v->fields[idx]->name->value;
                _out<<"template <> inline constexpr std::array fieldName<"<<name<<", "<<idx<<"> = std::to_array(\""<<fname<<"\");"<<el;
                _out<<"template <> inline constexpr "<<name<<" fieldValue<"<<name<<", "<<idx<<"> = "<<name<<"::"<<fname<<";"<<el;
            }
        };

        auto processorStruct = [&](const std::string& name, const SStruct* v)
        {
            _out<<"template <> inline constexpr "<<nameInIntrospection("Kind")<<" kind<"<<name<<"> = "<<nameInIntrospection("Kind")<<"::struct_;"<<el;

            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" basesCount<"<<name<<"> = "<<v->bases.size()<<";"<<el;
            for(std::size_t idx(0); idx<v->bases.size(); ++idx)
            {
                std::string btype;
                {
                    _out.push();
                    walk(v->bases[idx]->scopedName);
                    btype = _out.pop(false);
                }

                _out<<"template <> struct BaseType<"<<name<<", "<<idx<<"> {using result = "<<btype<<"; };"<<el;
            }

            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" fieldsCount<"<<name<<"> = "<<v->fields.size()<<";"<<el;
            for(std::size_t idx(0); idx<v->fields.size(); ++idx)
            {
                std::string fname = v->fields[idx]->name->value;
                std::string ftype;
                {
                    _out.push();
                    walk(v->fields[idx]->type);
                    ftype = _out.pop(false);
                }

                _out<<"template <> inline constexpr std::array fieldName<"<<name<<", "<<idx<<"> = std::to_array(\""<<fname<<"\");"<<el;
                _out<<"template <> struct FieldType<"<<name<<", "<<idx<<"> {using result = "<<ftype<<"; };"<<el;
                _out<<"template <> inline constexpr auto fieldValue<"<<name<<", "<<idx<<"> = memberValue<"<<name<<", &"<<name<<"::"<<fname<<">;"<<el;
            }
        };

        auto processorException = [&](const std::string& name, const SException* v)
        {
            _out<<"template <> inline constexpr "<<nameInIntrospection("Kind")<<" kind<"<<name<<"> = "<<nameInIntrospection("Kind")<<"::exception;"<<el;

            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" basesCount<"<<name<<"> = "<<(v->base ? "1" : "0")<<";"<<el;
            if(v->base)
            {
                std::string btype;
                {
                    _out.push();
                    walk(v->base->scopedName);
                    btype = _out.pop(false);
                }

                _out<<"template <> struct BaseType<"<<name<<", 0> {using result = "<<btype<<"; };"<<el;
            }

            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" fieldsCount<"<<name<<"> = "<<v->fields.size()<<";"<<el;
            for(std::size_t idx(0); idx<v->fields.size(); ++idx)
            {
                std::string fname = v->fields[idx]->name->value;
                std::string ftype;
                {
                    _out.push();
                    walk(v->fields[idx]->type);
                    ftype = _out.pop(false);
                }

                _out<<"template <> inline constexpr std::array fieldName<"<<name<<", "<<idx<<"> = std::to_array(\""<<fname<<"\");"<<el;
                _out<<"template <> struct FieldType<"<<name<<", "<<idx<<"> {using result = "<<ftype<<"; };"<<el;
                _out<<"template <> inline constexpr auto fieldValue<"<<name<<", "<<idx<<"> = memberValue<"<<name<<", &"<<name<<"::"<<fname<<">;"<<el;
            }
        };

        auto processorInterface = [&](const std::string& name_, const SInterface* v)
        {
            (void)v;
            const std::string side = nameInSparesI(v->isPrimary?"Side::primary":"Side::opposite");
            const std::string name = name_+"<"+side+">";

            _out<<"template <> inline constexpr "<<nameInIntrospection("Kind")<<" kind<"<<name<<"> = "<<nameInIntrospection("Kind")<<"::interface;"<<el;

            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" basesCount<"<<name<<"> = "<<v->bases.size()<<";"<<el;
            for(std::size_t idx(0); idx<v->bases.size(); ++idx)
            {
                std::string btype;
                {
                    _out.push();
                    walk(v->bases[idx]->scopedName);
                    btype = _out.pop(false);
                }

                _out<<"template <> struct BaseType<"<<name<<", "<<idx<<"> {using result = "<<btype<<"; };"<<el;
            }

            std::vector<MethodFromBaseOrSelf> methods = collectMethodsFromBasesOrSelf(v);
            std::size_t methodsCount = 0;
            for(const MethodFromBaseOrSelf& m : methods)
            {
                if(m._overrided)
                {
                    continue;
                }
                ++methodsCount;
            }
            _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" methodsCount<"<<name<<"> = "<<methodsCount<<";"<<el;

            std::size_t idx = 0;
            for(const MethodFromBaseOrSelf& m : methods)
            {
                if(m._overrided)
                {
                    continue;
                }
                const Method& method = m._method;

                _out<<"template <> inline constexpr std::array methodName<"<<name<<", "<<idx<<"> = std::to_array(\""<<method->name->value<<"\");"<<el;

                //_out<<"template <> inline constexpr auto methodActivator<"<<name<<", "<<idx<<"> = [](const "<<name<<"& i, auto&&... args) -> decltype(auto) {";





                _out<<"template <> inline constexpr auto methodActivator<"<<name<<", "<<idx<<"> = [](const "<<name<<"& i";

                if(MethodDirection::in == method->direction)
                {
                    std::size_t argIndex = 0;
                    for(MethodParam& param : method->query)
                    {
                        _out<<", ";
                        writeMethodParamType(param, argIndex);
                        writeMethodParamName(param, argIndex);

                        argIndex++;
                    }

                    _out<<") -> decltype(auto) {return i->"<<method->name->value<<"(";

                    argIndex = 0;
                    for(MethodParam& param : method->query)
                    {
                        if(argIndex) _out<<", ";
                        _out<<"std::forward<decltype(";
                        writeMethodParamName(param, argIndex);
                        _out<<")>(";
                        writeMethodParamName(param, argIndex);
                        _out<<")";

                        argIndex++;
                    }
                    _out<<");";
                }
                else
                {
                    _out<<") -> decltype(auto) {return i->"<<method->name->value<<"();";
                }

                _out<<"};"<<el;

                _out<<"template <> inline constexpr bool methodIn<"<<name<<", "<<idx<<"> = "<<(MethodDirection::in == method->direction ? "true" : "false")<<";"<<el;
                _out<<"template <> inline constexpr bool methodNoReply<"<<name<<", "<<idx<<"> = "<<(method->noreply ? "true" : "false")<<";"<<el;

                if(!method->noreply)
                {
                    std::string resultType;
                    {
                        _out.push();
                        walk(method->reply);
                        resultType = _out.pop(false);
                    }

                    _out<<"template <> struct MethodReplyType<"<<name<<", "<<idx<<"> {using result = "<<resultType<<"; };"<<el;
                }

                _out<<"template <> inline constexpr "<<nameInPrimitives("uint32")<<" methodParamsCount<"<<name<<", "<<idx<<"> = "<<method->query.size()<<";"<<el;
                for(std::size_t pidx(0); pidx<method->query.size(); ++pidx)
                {
                    const ast::MethodParam& mp = method->query[pidx];

                    std::string paramName;
                    {
                        _out.push();
                        writeMethodParamName(mp, pidx);
                        paramName = _out.pop(false);
                    }
                    _out<<"template <> inline constexpr std::array methodParamName<"<<name<<", "<<idx<<", "<<pidx<<"> = std::to_array(\""<<paramName<<"\");"<<el;

                    std::string paramType;
                    {
                        _out.push();
                        walk(mp->type);
                        paramType = _out.pop(false);
                    }
                    _out<<"template <> struct MethodParamType<"<<name<<", "<<idx<<", "<<pidx<<"> {using result = "<<paramType<<"; };"<<el;
                }

                ++idx;
            }

        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& v : s->flagses)
            {
                std::string name = processorCommon(v);
                processorFlags(name, v);
            }

            for(const auto& v : s->enums)
            {
                std::string name = processorCommon(v);
                processorEnum(name, v);
            }

            for(const auto& v : s->interfaces)
            {
                std::string name = processorCommon(v);
                processorInterface(name, v);
                processorInterface(name, v->opposite);
            }

            for(const auto& v : s->structs)
            {
                std::string name = processorCommon(v);
                processorStruct(name, v);
            }

            for(const auto& v : s->exceptions)
            {
                std::string name = processorCommon(v);
                processorException(name, v);
            }

            for(const auto& v : s->scopes)
            {
                self(self, v);
            }
        };

        walker(walker, v.get());

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeBundles(const im::ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//bundle for "<<v->prepareFullScopedName()->toString()<<el;
            _out<<"template <>"<<el;
            _out<<"struct WiresBundle<"; walk(v->prepareFullScopedName()); _out<<">"<<el;
            auto _=_out.scope("{", "};");

            for(const MethodFromBaseOrSelf& m : collectMethodsFromBasesOrSelf(v))
            {
                if(m._method->owner != v)
                {
                    continue;
                }

                if(m._overrided)
                {
                    _out<<"// ";
                }

                auto method = m._method;

                _out<<nameInSbs("Wire")<<"<"<<nameInSparesC("Res")<<"<";
                if(method->noreply)
                {
                    _out<< nameInSparesC("NoReply");
                }
                else
                {
                    walk(method->reply);
                }
                _out<<">";

                for(MethodParam& param : method->query)
                {
                    _out<<", ";
                    walk(param->type);
                }

                _out<<"> "<<m._method->name->value<<";"<<el;
            }
        };

        enumerateInterfaces(v.get(), processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeBundleContainers(const im::ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//bundles container for "<<v->prepareFullScopedName()->toString()<<el;
            _out<<"template <>"<<el;
            _out<<"struct WiresBundlesContainer<"; walk(v->prepareFullScopedName()); _out<<">"<<el;
            auto _=_out.scope("{", "};");

            for(const BaseOrSelf& bos : collectBasesOrSelf(v))
            {
                _out<<"WiresBundle<"; walk(bos._interface->prepareFullScopedName()); _out<<"> "<<bundleInContainerName(bos._interface)<<";"<<el;
            }

        };

        enumerateInterfaces(v.get(), processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeHmDescriptors(const im::ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            auto boses = collectBasesOrSelf(v);
            for(std::size_t index(0); index < boses.size(); ++index)
            {
                const BaseOrSelf& bos = boses[index];

                _out<<el;
                _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
                _out<<"//hierarchy member descriptor for "<<v->prepareFullScopedName()->toString()<<el;
                _out<<"template <>"<<el;
                _out<<"struct HmDescriptor<";
                    walk(v->prepareFullScopedName());
                    _out<<", ";
                    walk(bos._interface->prepareFullScopedName());
                _out<<">"<<el;

                {
                    auto _=_out.scope("", "");
                    _out<<": public "<<nameInSparesC("HmDescriptorBase")<<""<<el;
                }

                auto _=_out.scope("{", "};");
                {
                    _out<<"HmDescriptor();"<<el;
                    _out<<"static constexpr bool _declared = true;"<<el;
                    _out<<"static constexpr "<<nameInPrimitives("uint32")<<" _index = "<<index<<";"<<el;
                    _out<<"static constexpr bool _mdiSideSame = "<<(bos._interface->isPrimary == v->isPrimary ? "true":"false") <<";"<<el;

                    std::vector<BaseOrSelf> basesOrSelf2 = collectBasesOrSelf(bos._interface);

                    _out<<"const HmbDescriptor _hmbds["<<basesOrSelf2.size()<<"];"<<el;
                }
            }
        };

        enumerateInterfaces(v.get(), processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeHmDescriptorMethods(const im::ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            std::vector<Cpp::BaseOrSelf> boses = collectBasesOrSelf(v);
            for(const BaseOrSelf& bos : boses)
            {
                _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
                _out<<"inline HmDescriptor<"; walk(v->prepareFullScopedName()); _out<<", "; walk(bos._interface->prepareFullScopedName()); _out<<">::HmDescriptor()"<<el;

                {
                    auto _=_out.scope("", "");
                    _out<<": "<<nameInSparesC("HmDescriptorBase")<<"(_mdiSideSame)"<<el;
                    _out<<", _hmbds"<<el;

                    {
                        auto _0=_out.scope("", "");
                        auto _=_out.scope();

                        for(const BaseOrSelf& bos2 : collectBasesOrSelf(bos._interface))
                        {
                            auto _=_out.scope("{", "},");
                            _out<<"offsetof("<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">, "<<bundleInContainerName(bos2._interface)<<"),"<<el;

                            if(bos._interface == bos2._interface)
                            {
                                _out<<"this"<<el;
                            }
                            else
                            {
                                _out<<"&hmDescriptor<"; walk(v->prepareFullScopedName()); _out<<", "; walk(bos2._interface->prepareFullScopedName()); _out<<">"<<el;
                            }
                        }
                    }
                }

                auto _=_out.scope();
            }
        };

        enumerateInterfaces(v.get(), processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeMdDescriptors(const im::ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//most derived interface descriptor for "<<v->prepareFullScopedName()->toString()<<el;
            _out<<"template <>"<<el;
            _out<<"struct MdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">"<<el;

            {
                auto _=_out.scope("", "");
                _out<<": public "<<nameInSparesC("MdDescriptorBase")<<""<<el;
            }

            {
                auto _=_out.scope("{", "};");
                {
                    _out<<"static constexpr bool _declared = true;"<<el;

                    _out<<"MdDescriptor();"<<el;

                    _out<<"static "<<nameInSparesC("MdState")<<"* createMds();"<<el;
                    _out<<"static void disconnectWires("<<nameInSparesC("MdState")<<"* mds);"<<el;
                    _out<<"static void destroyMds("<<nameInSparesC("MdState")<<"* mds);"<<el;
                    _out<<"static const "<<nameInSparesC("HmDescriptorBase")<<"* resolveHmd(const "<<nameInSparesC("Id")<<"& id, const "<<nameInSparesC("Lid")<<"& lid);"<<el;

                    _out<<"static constexpr "<<nameInSparesC("Id")<<" _id {"<<idInitializer(v->sign4Name ^ v->sign4Layout)<<"};"<<el;
                }
            }
        };

        enumerateInterfaces(v.get(), processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeMdDescriptorMethods(const im::ast::Scope& v)
    {
        auto writeHmdResolverBody = [&](const SInterface* v)
        {
            std::vector<BaseOrSelf> boses = collectBasesOrSelf(v);

            //single
            if(1 == boses.size())
            {
                _out<<"(void)id;"<<el;
                _out<<"if(mdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">._lid == lid)"<<el;
                {
                    auto _=_out.scope();
                    _out<<"return &hmDescriptor<"; walk(v->prepareFullScopedName()); _out<<", "; walk(v->prepareFullScopedName()); _out<<">;"<<el;
                }
                _out<<"return nullptr;"<<el;
                return;
            }

            //multiple - mphf throuh switch
            {
                for(std::size_t bytes(1); bytes<=8; bytes*=2)
                {

                    for(std::size_t idx(0); idx<Sign::_size/bytes; ++idx)
                    {
                        std::set<std::string> values;

                        for(const BaseOrSelf& bos : boses)
                        {
                            values.insert((bos._interface->sign4Name ^ bos._interface->sign4Layout).toHex(idx*bytes*2, bytes*2));
                        }

                        if(values.size() == boses.size())
                        {
                            _out<<"switch(id.asArray<uint"<<(bytes*8)<<">()["<<idx<<"])"<<el;
                            {
                                _out<<"{"<<el;
                                for(const BaseOrSelf& bos : boses)
                                {
                                    _out<<"case "<<nameInSparesC("MdDescriptor")<<"<"; walk(bos._interface->prepareFullScopedName()); _out<<">::_id.asArray<uint"<<(bytes*8)<<">()["<<idx<<"]:"<<el;
                                    auto _=_out.scope("", "");

                                    _out<<"if(mdDescriptor<"; walk(bos._interface->prepareFullScopedName()); _out<<">._lid == lid)"<<el;
                                    {
                                        auto _=_out.scope();
                                        _out<<"return &hmDescriptor<"; walk(v->prepareFullScopedName()); _out<<", "; walk(bos._interface->prepareFullScopedName()); _out<<">;"<<el;
                                    }
                                    _out<<"break;"<<el;
                                }
                                _out<<"}"<<el;
                            }

                            _out<<"return nullptr;"<<el;
                            return;
                        }
                    }
                }
            }

            //multiple - conditions serie
            {
                _out<<"(void)id;"<<el;
                for(const BaseOrSelf& bos : boses)
                {
                    _out<<"if(mdDescriptor<"; walk(bos._interface->prepareFullScopedName()); _out<<">._lid == lid)"<<el;
                    auto _=_out.scope();
                    _out<<"return &hmDescriptor<"; walk(v->prepareFullScopedName()); _out<<", "; walk(bos._interface->prepareFullScopedName()); _out<<">;"<<el;
                }
                _out<<"return nullptr;"<<el;
            }
        };

        auto processor = [&](const SInterface* v)
        {

            _out<<"inline MdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">::MdDescriptor()"<<el;
            {
                {
                    auto _=_out.scope("", "");
                    _out<<": "<<nameInSparesC("MdDescriptorBase")<<"(&MdDescriptor::createMds, &MdDescriptor::disconnectWires, &MdDescriptor::destroyMds, &MdDescriptor::resolveHmd, _id, "<<nameInSparesC("lidRegistry")<<".emplace(_id))"<<el;
                }
                auto _=_out.scope();
            }

            _out<<"inline "<<nameInSparesC("MdState")<<"* MdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">::createMds()"<<el;
            {
                auto _=_out.scope();

                _out<<"static_assert(alignof("<<nameInSparesC("MdState")<<") >= alignof("<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">));"<<el;
                _out<<"const "<<nameInPrimitives("uint32")<<" size = sizeof("<<nameInSparesC("MdState")<<") + sizeof("<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">);"<<el;
                _out<<""<<nameInSparesC("MdState")<<"* mds = new (mm::heap::alloc<size>()) "<<nameInSparesC("MdState")<<"{mdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">};"<<el;
                _out<<"new (&mds->getWiresBundlesContainer<char>()) "<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">{};"<<el;
                _out<<"return mds;"<<el;
            }

            _out<<"inline void MdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">::disconnectWires("<<nameInSparesC("MdState")<<"* mds)"<<el;
            {
                auto _=_out.scope();

                bool wbcDeclared = false;
                for(const MethodFromBaseOrSelf& m : collectMethodsFromBasesOrSelf(v))
                {
                    if(m._overrided)
                    {
                        continue;
                    }

                    if(!wbcDeclared)
                    {
                        _out<<""<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">& wbc = mds->getWiresBundlesContainer<"<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">>();"<<el;
                        wbcDeclared = true;
                    }

                    _out<<"wbc."<<bundleInContainerName(m._method->owner)<<"."<<m._method->name->value<<".disconnectAll();"<<el;
                }

                if(!wbcDeclared)
                {
                    _out<<"(void)mds;"<<el;
                }
            }

            _out<<"inline void MdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">::destroyMds("<<nameInSparesC("MdState")<<"* mds)"<<el;
            {
                auto _=_out.scope();
                _out<<"mds->getWiresBundlesContainer<"<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">>().~WiresBundlesContainer();"<<el;
                _out<<"mds->~MdState();"<<el;
                _out<<"const "<<nameInPrimitives("uint32")<<" size = sizeof("<<nameInSparesC("MdState")<<") + sizeof("<<nameInSparesC("WiresBundlesContainer")<<"<"; walk(v->prepareFullScopedName()); _out<<">);"<<el;
                _out<<"mm::heap::free<size>(mds);"<<el;
            }

            _out<<"inline const "<<nameInSparesC("HmDescriptorBase")<<"* MdDescriptor<"; walk(v->prepareFullScopedName()); _out<<">::resolveHmd(const "<<nameInSparesC("Id")<<"& id, const "<<nameInSparesC("Lid")<<"& lid)"<<el;
            {
                auto _=_out.scope();
                writeHmdResolverBody(v);
            }
        };

        enumerateInterfaces(v.get(), processor);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeInterfaceMethodBodies(const ast::Scope &v)
    {
        auto processorInterface = [&](const SInterface* v)
        {
            std::string className = v->name->value+"<"+nameInSparesI(v->isPrimary?"Side::primary":"Side::opposite")+">";
            std::string baseName = nameInSparesI("ImplBase")+"<"+v->name->value+", "+nameInSparesI(v->isPrimary?"Side::primary":"Side::opposite")+">";

            _out<<"inline "<<className<<"::"<<v->name->value<<"(const "<<v->name->value<<"& from): "<<baseName<<"(from){}"<<el;
            _out<<"inline "<<className<<"::"<<v->name->value<<"("<<v->name->value<<"&& from): "<<baseName<<"(std::move(from)){}"<<el;

            _out<<"inline "<<className<<"& "<<className<<"::operator=(const "<<v->name->value<<"& from){return "<<baseName<<"::operator=(from);}"<<el;
            _out<<"inline "<<className<<"& "<<className<<"::operator=("<<v->name->value<<"&& from){return "<<baseName<<"::operator=(std::move(from));}"<<el;

        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& s : s->scopes)
            {
                if(s->owner)
                {
                    _out<<"namespace "<<s->name->value<<el;
                    auto _=_out.scope();
                    self(self, s);
                }
            }
            for(const auto& v : s->interfaces)
            {
                processorInterface(v);
                processorInterface(v->opposite);
            }
        };

        walker(walker, v.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeMethods(const im::ast::Scope& v)
    {
        auto processor = [&](const std::string& cname, const SInterface* v)
        {
            const std::string sname = nameInSparesI(v->isPrimary?"Side::primary":"Side::opposite");
            const std::string baseName = nameInSparesI("MethodsBase")+"<"+cname+", "+sname+">";
            _out<<"template <>"<<el;
            _out<<"struct Methods<"<<cname<<", "<<sname<<">"<<el;

            {
                auto _=_out.scope("", "");

                _out<<": public "<<baseName<<el;
            }

            auto _=_out.scope("{", "};");

            for(const MethodFromBaseOrSelf& m : collectMethodsFromBasesOrSelf(v))
            {
                if(m._overrided)
                {
                    continue;
                    //_out<<"// ";
                }

                writeMethodSignature(m);
                _out<<el;
                auto _=_out.scope();

                _out<<"return "<<baseName<<"::_stateHolder.getWiresBundle<"<<nameInGen(m._method->owner->prepareFullScopedName()->toString())<<">()."<<m._method->name->value;

                if(MethodDirection::in == m._method->direction)
                {
                    _out<<".in2";
                    if(!m._method->noreply)
                    {
                        _out<<"<agg>";
                    }

                    _out<<"("<<nameInSparesC("ResAdapter")<<"()";

                    std::size_t argIndex = 0;
                    for(MethodParam& param : m._method->query)
                    {
                        _out<<", std::forward<decltype(";
                        writeMethodParamName(param, argIndex);
                        _out<<")>(";
                        writeMethodParamName(param, argIndex);
                        _out<<")";
                        argIndex++;
                    }

                    _out<<")";
                }
                else
                {
                    _out<<".out()";
                }

                _out<<";"<<el;
            }
        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& s : s->scopes)
            {
                self(self, s);
            }
            for(const auto& v : s->interfaces)
            {
                std::string cname;
                {
                    _out.push();
                    walk(v->prepareFullScopedName());
                    cname = _out.pop(false);
                }

                _out<<"//methods for primary interface "<<cname<<el;
                processor(cname, v);
                _out<<"//methods for opposite interface "<<cname<<el;
                processor(cname, v->opposite);
            }
        };

        walker(walker, v.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeStructAndExceptionOperators(const ast::Scope& v)
    {
        auto processor = [&](const auto* v)
        {
            std::string lhsFields, rhsFields;

            for(const auto& field : v->fields)
            {
                if(!lhsFields.empty())
                {
                    lhsFields += ", ";
                    rhsFields += ", ";
                }

                lhsFields += "this->" + field->name->value;
                rhsFields += "rhs." + field->name->value;
            }

            _out<<"inline std::partial_ordering "<<v->name->value<<"::operator<=>(const "<<v->name->value<<"& rhs) const"<<el;
            {
                auto _=_out.scope();
                _out<<"(void)rhs;"<<el;
                _out<<"return std::tie("<<lhsFields<<") <=> std::tie("<<rhsFields<<");"<<el;
            }

            _out<<"inline bool "<<v->name->value<<"::operator==(const "<<v->name->value<<"& rhs) const"<<el;
            {
                auto _=_out.scope();
                _out<<"(void)rhs;"<<el;
                _out<<"return std::tie("<<lhsFields<<") == std::tie("<<rhsFields<<");"<<el;
            }
        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& s : s->scopes)
            {
                if(s->owner)
                {
                    _out<<"namespace "<<s->name->value<<el;
                    auto _=_out.scope();
                    self(self, s);
                }
            }
            for(const auto& v : s->structs)
            {
                processor(v);
            }
            for(const auto& v : s->exceptions)
            {
                processor(v);
            }
        };

        walker(walker, v.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeStiacLinks(const ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            std::string cname;
            {
                _out.push();
                walk(v->prepareFullScopedName());
                cname = _out.pop(false);
            }

            std::string sname = nameInSparesI("Side::")+(v->isPrimary?"primary":"opposite");

            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//stiac link for "<<cname<<el;

            if(!std::regex_search(cname, _stiacSupportFilter))
            {
                _out<<"//filtered"<<el;
                return;
            }

            _out<<"template <>"<<el;
            _out<<"class Impl<"<<cname<<"<"<<sname<<">> final"<<el;

            {
                auto _=_out.scope("", "");
                _out<<": public "<<nameInStiac("link::InterfaceBase")<<"<"<<cname<<", "<<sname<<">"<<el;
            }

            {
                auto _=_out.scope("{", "};");

                _out<<undent;
                _out<<"public:"<<el;
                _out<<indent;
                _out<<"static constexpr bool _declared = true;"<<el;

                _out<<undent;
                _out<<"private:"<<el;
                _out<<indent;

                _out<<"using "<<nameInStiac("link::InterfaceBase")<<"<"<<cname<<", "<<sname<<">::InterfaceBase;"<<el;
                _out<<"~Impl() override;"<<el;
                _out<<"void initialize("<<nameInStiac("link::Hub4Link")<<"* hub, "<<nameInStiac("link::Id")<<" id) override;"<<el;
                _out<<"void input("<<nameInStiac("link::Source")<<"& source) override;"<<el;
                _out<<"void destroy() override;"<<el;
            }
        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& s : s->scopes)
            {
                self(self, s);
            }
            for(const auto& i : s->interfaces)
            {
                processor(i);
                processor(i->opposite);
            }
        };

        walker(walker, v.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeStiacLinkMethodBodies(const ast::Scope& v)
    {
        auto processor = [&](const SInterface* v)
        {
            std::string cname;
            {
                _out.push();
                walk(v->prepareFullScopedName());
                cname = _out.pop(false);
            }

            std::string sname = nameInSparesI("Side::")+(v->isPrimary?"primary":"opposite");

            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//stiac link method bodies for "<<cname<<el;

            if(!std::regex_search(cname, _stiacSupportFilter))
            {
                _out<<"//filtered"<<el;
                return;
            }

            std::string implName = "Impl<"+cname+"<"+sname+">>";

            std::vector<MethodFromBaseOrSelf> methodsFromBasesOrSelf = collectMethodsFromBasesOrSelf(v);

            ///////////////////////
            ///////////////////////
            ///////////////////////
            // dtor
            _out<<"inline "<<implName<<"::~Impl()"<<el;
            {
                auto _ = _out.scope();
                _out<<"_sbsOwner.flush();"<<el;
            }

            ///////////////////////
            ///////////////////////
            ///////////////////////
            // initialize
            _out<<"inline void "<<implName<<"::initialize("<<nameInStiac("link::Hub4Link")<<"* hub, "<<nameInStiac("link::Id")<<" id)"<<el;
            {
                auto _ = _out.scope();

                _out<<""<<nameInStiac("link::InterfaceBase")<<"<"<<cname<<", "<<sname<<">::initialize(hub, id);"<<el;

                std::uint32_t inMethodIndex = 0;
                for(const MethodFromBaseOrSelf& m : methodsFromBasesOrSelf)
                {
//                    if(m._overrided)
//                    {
//                        continue;
//                    }

                    if(MethodDirection::in != m._method->direction)
                    {
                        _out<<"this->"<<nameInStiac("link::InterfaceBase")<<"<"<<cname<<", "<<sname<<">::getWiresBundle<"<<nameInGen(m._method->owner->prepareFullScopedName()->toString())<<">()";
                        _out<<"."<<m._method->name->value;

                        _out<<".out() += _sbsOwner * [this](";

                        std::size_t argIndex{0};
                        for(const MethodParam& param : m._method->query)
                        {
                            if(argIndex)
                            {
                                _out<<", ";
                            }

                            _out<<nameInSparesC("Arg")<<"<";
                            walk(param->type);
                            _out<<", "<<argIndex<<"> auto&&";

                            writeMethodParamName(param, argIndex);
                            argIndex++;
                        }
                        _out<<") -> ";

                        if(m._method->noreply)
                        {
                            _out<< nameInSparesC("Res")<<"<"<<nameInSparesC("NoReply")<<">";
                        }
                        else
                        {
                            _out<< nameInSparesC("Res")<<"<";
                            walk(m._method->reply);
                            _out<<">";
                        }
                        _out<<el;

                        {
                            auto _ = _out.scope("{", "};");

                            _out<<"return this->"<<nameInStiac("link::InterfaceBase")<<"<"<<cname<<", "<<sname<<">::call2Bin<"<<nameInSparesC("Res")<<"<";
                            if(m._method->noreply)
                            {
                                _out<< nameInSparesC("NoReply");
                            }
                            else
                            {
                                walk(m._method->reply);
                            }
                            _out<<">>("<<nameInStiac("link::MethodId")<<"{"<<inMethodIndex<<"}";
                            std::size_t argIndex{0};
                            for(const MethodParam& param : m._method->query)
                            {
                                _out<<", std::forward<decltype(";
                                writeMethodParamName(param, argIndex);
                                _out<<")>(";
                                writeMethodParamName(param, argIndex);
                                _out<<")";
                                argIndex++;
                            }
                            _out<<");"<<el;
                        }

                        inMethodIndex++;
                    }
                }
            }

            ///////////////////////
            ///////////////////////
            ///////////////////////
            // input
            _out<<"inline void "<<implName<<"::input("<<nameInStiac("link::Source")<<"& source)"<<el;
            {
                auto _ = _out.scope();

                _out<<""<<nameInStiac("link::MethodId")<<" methodId;"<<el;
                _out<<"source >> methodId;"<<el;

                _out<<"switch(static_cast<"<<nameInPrimitives("uint32")<<">(methodId))"<<el;
                _out<<"{"<<el;

                std::uint32_t inMethodIndex = 0;
                for(const MethodFromBaseOrSelf& m : methodsFromBasesOrSelf)
                {
//                    if(m._overrided)
//                    {
//                        continue;
//                    }

                    if(MethodDirection::in == m._method->direction)
                    {
                        _out<<"case "<<inMethodIndex<<":"<<el;
                        {
                            auto _  = _out.scope("", "");

                            _out<<"return bin2Call(source, ";

                            _out<<"this->"<<nameInStiac("link::InterfaceBase")<<"<"<<cname<<", "<<sname<<">::getWiresBundle<"<<nameInGen(m._method->owner->prepareFullScopedName()->toString())<<">()";
                            _out<<"."<<m._method->name->value;
                            _out<<");"<<el;

                        }

                        inMethodIndex++;
                    }
                }
                _out<<"default:"<<el;
                {
                    auto _  = _out.scope("", "");

                    _out<<"return source.fail(\"malformed input\");"<<el;
                }

                _out<<"}"<<el;
            }

            ///////////////////////
            ///////////////////////
            ///////////////////////
            // destroyer
            _out<<"inline void "<<implName<<"::destroy()"<<el;
            {
                auto _ = _out.scope();
                _out<<"delete this;"<<el;
            }
        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& s : s->scopes)
            {
                self(self, s);
            }
            for(const auto& i : s->interfaces)
            {
                processor(i);
                processor(i->opposite);
            }
        };

        walker(walker, v.get());
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeStiacExceptionSerializers(const ast::Scope &v)
    {
        auto processor = [&](const SException* v)
        {
            std::string name;
            {
                _out.push();
                walk(v->prepareFullScopedName());
                name = _out.pop(false);
            }

            _out<<el;
            _out<<"/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7"<<el;
            _out<<"//stiac exception serializer for "<<name<<el;

            if(!std::regex_search(name, _stiacSupportFilter))
            {
                _out<<"//filtered"<<el;
                return;
            }

            _out<<"template class ExceptionSerializerRegistrator<"<<name<<">;"<<el;
        };

        auto walker = [&](const auto& self, const SScope* s) -> void
        {
            for(const auto& s : s->scopes)
            {
                self(self, s);
            }
            for(const auto& e : s->exceptions)
            {
                processor(e);
            }
        };

        walker(walker, v.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInRoot(const std::string& name)
    {
        return _rootScopeName+(':'==name[0] ?  "" : "::")+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInGen(const std::string& name)
    {
        return _genScopeName+(':'==name[0] ?  "" : "::")+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInSparesI(const std::string& name)
    {
        return _sparesIScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInSparesC(const std::string& name)
    {
        return _sparesCScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInIntrospection(const std::string& name)
    {
        return _introspectionScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInPrimitives(const std::string& name)
    {
        return _primitivesScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInSbs(const std::string& name)
    {
        return _sbsScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInStiac(const std::string& name)
    {
        return _stiacScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::nameInDci(const std::string& name)
    {
        return _dciScopeName+"::"+name;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::bundleInContainerName(const SInterface* i)
    {
        auto fullScopedName = i->prepareFullScopedName();
        fullScopedName->root = false;

        std::string sname = fullScopedName->toString();
        std::replace(sname.begin(), sname.end(), ':', '_');
        return sname;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Cpp::idInitializer(const Sign& sign)
    {
        std::string res = "{";
        for(std::size_t idx(0); idx<16; ++idx)
        {
            if(idx)
            {
                res += ",";
            }
            res += "0x"+sign.toHex(idx*2+1, 1)+sign.toHex(idx*2+0, 1);
        }
        res += "}";
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::vector<Cpp::MethodFromBaseOrSelf> Cpp::collectMethodsFromBasesOrSelf(const SInterface* v)
    {
        std::vector<Cpp::MethodFromBaseOrSelf> res;
        std::set<const SInterface*> processed;
        std::set<std::string> processedMethods;

        auto processor = [&](const auto& self, const SInterface* v) -> void
        {
            if(processed.end() == processed.find(v))
            {
                processed.insert(v);

                for(const Method& m : v->methods)
                {
                    MethodFromBaseOrSelf desc;
                    desc._method = m;
                    desc._overrided = processedMethods.end() != processedMethods.find(m->name->value);

                    processedMethods.insert(m->name->value);
                    res.emplace_back(desc);
                }
            }

            for(const InterfaceBase& b : v->bases)
                self(self, b->instance);
        };

        processor(processor, v);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::vector<Cpp::BaseOrSelf> Cpp::collectBasesOrSelf(const im::ast::SInterface* v)
    {
        std::vector<BaseOrSelf> res;
        std::set<const SInterface*> processed;

        auto processor = [&](const auto& self, const SInterface* v) -> void
        {
            if(!processed.insert(v).second)
                return;

            for(const InterfaceBase& b : v->bases)
                self(self, b->instance);

            res.emplace_back(BaseOrSelf{v});
        };

        processor(processor, v);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeMethodParamType(const ast::MethodParam& param, std::size_t index)
    {
        _out<<nameInSparesC("Arg")<<"<";
        walk(param->type);
        _out<<", "<<index<<"> auto&& ";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeMethodParamName(const ast::MethodParam& param, std::size_t index)
    {
        if(param->name && !param->name->value.empty())
        {
            _out<<param->name->value;
        }
        else
        {
            _out<<"arg"<<index;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cpp::writeMethodSignature(const MethodFromBaseOrSelf& m)
    {
        std::string classScope;

        const Method& method = m._method;
        if(MethodDirection::in == method->direction)
        {
            if(!method->noreply)
            {
                _out<<"template <"<<nameInSbs("wire::Agg")<<" agg = "<<nameInSbs("wire::Agg")<<"::first";
                _out<<"> ";
            }

            _out<<nameInSparesC("Res")<<"<";
            if(method->noreply)
            {
                _out<< nameInSparesC("NoReply");
                _out<<">";
            }
            else
            {
                walk(method->reply);
                _out<<", agg>";
            }
            _out<<" "<<classScope<<method->name->value<<"(";

            std::size_t argIndex = 0;
            for(MethodParam& param : method->query)
            {
                if(argIndex) _out<<", ";

                writeMethodParamType(param, argIndex);
                writeMethodParamName(param, argIndex);

                argIndex++;
            }

            _out<<") const";
        }
        else
        {
            _out<<"[[nodiscard]] ";

            _out<<nameInSbs("Signal")<<"<"<<nameInSparesC("Res")<<"<";
            if(method->noreply)
            {
                _out<< nameInSparesC("NoReply");
            }
            else
            {
                walk(method->reply);
            }
            _out<<">";

            for(MethodParam& param : method->query)
            {
                _out<<", ";
                walk(param->type);
            }

            _out<<">";
            _out<<" "<<classScope<<method->name->value<<"() const";
        }
    }
}

