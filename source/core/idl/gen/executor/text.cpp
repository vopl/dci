// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "text.hpp"
#include <iostream>

namespace dci::idl::gen::executor
{
    using namespace im::ast;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Text::Text()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Text::~Text()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    boost::program_options::options_description Text::options()
    {
        boost::program_options::options_description local(name() + " specific options");
        local.add_options()
                (
                    (name()+"-indent").data(),
                    boost::program_options::value<std::string>()->default_value("    "),
                    "specify indent value"
                )
                (
                    (name()+"-output-file").data(),
                    boost::program_options::value<std::string>(),
                    "output file name. If absent text will be writen to stdout"
                )
                ;

        return local;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Text::run(const im::Storage& ims, const boost::program_options::variables_map& vars)
    {
        (void)ims;

        try
        {
            if(vars.count(name()+"-output-file"))
            {
                _out.open(vars[name()+"-output-file"].as<std::string>());
            }
            else
            {
                _out.open();
            }

            _out.setIndent(vars[name()+"-indent"].as<std::string>());
        }
        catch(std::exception& e)
        {
            std::cerr<<e.what()<<std::endl;
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Primitive& v)
    {
        (void)v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Array& v)
    {
        walk(v->elementType);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Tuple& v)
    {
        for(TypeUse& elementType : v->elementTypes)
        {
            walk(elementType);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Ptr& v)
    {
        walk(v->valueType);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Opt& v)
    {
        walk(v->valueType);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const List& v)
    {
        walk(v->elementType);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Map& v)
    {
        walk(v->keyType);
        walk(v->valueType);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Set& v)
    {
        walk(v->elementType);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Alias& v)
    {
        walk(v->type);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Enum& v)
    {
        for(EnumField& field : v->fields)
        {
            walk(field);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const EnumField& v)
    {
        (void)v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Flags& v)
    {
        for(FlagsField& field : v->fields)
        {
            walk(field);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const FlagsField& v)
    {
        (void)v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Scope& v)
    {
        for(Decl& decl : v->decls)
        {
            if(boost::get<Alias     >(&decl)) walk(boost::get<Alias     >(decl));
            if(boost::get<Struct    >(&decl)) walk(boost::get<Struct    >(decl));
            if(boost::get<Enum      >(&decl)) walk(boost::get<Enum      >(decl));
            if(boost::get<Flags     >(&decl)) walk(boost::get<Flags     >(decl));
            if(boost::get<Exception >(&decl)) walk(boost::get<Exception >(decl));
            if(boost::get<Interface >(&decl)) walk(boost::get<Interface >(decl));
            if(boost::get<Scope     >(&decl)) walk(boost::get<Scope     >(decl));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Variant& v)
    {
        for(TypeUse& elementType : v->elementTypes)
        {
            walk(elementType);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Exception& v)
    {
        if(v->base)
        {
            walk(v->base);
        }

        for(ExceptionField& field : v->fields)
        {
            walk(field);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const ExceptionBase& v)
    {
        (void)v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const ExceptionField& v)
    {
        walk(v->type);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Struct& v)
    {
        for(StructBase& base : v->bases)
        {
            walk(base);
        }

        for(StructField& field : v->fields)
        {
            walk(field);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const StructField& v)
    {
        walk(v->type);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const StructBase& v)
    {
        (void)v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Interface& v)
    {
        for(InterfaceBase& base : v->bases)
        {
            walk(base);
        }

        for(Method& method : v->methods)
        {
            walk(method);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const InterfaceBase& v)
    {
        (void)v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const Method& v)
    {
        for(MethodParam& param : v->query)
        {
            walk(param);
        }

        if(!v->noreply)
        {
            walk(v->reply);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const MethodParam& v)
    {
        walk(v->type);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const TypeUse& v)
    {
        if(boost::get<Primitive     >(&v)) walk(boost::get<Primitive    >(v));
        if(boost::get<Tuple         >(&v)) walk(boost::get<Tuple        >(v));
        if(boost::get<List          >(&v)) walk(boost::get<List         >(v));
        if(boost::get<Set           >(&v)) walk(boost::get<Set          >(v));
        if(boost::get<Map           >(&v)) walk(boost::get<Map          >(v));
        if(boost::get<Ptr           >(&v)) walk(boost::get<Ptr          >(v));
        if(boost::get<Opt           >(&v)) walk(boost::get<Opt          >(v));
        if(boost::get<Array         >(&v)) walk(boost::get<Array        >(v));
        if(boost::get<Variant       >(&v)) walk(boost::get<Variant        >(v));
        if(boost::get<ScopedName    >(&v)) walk(boost::get<ScopedName   >(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Text::walk(const im::ast::ScopedName& v)
    {
        (void)v;
    }
}

