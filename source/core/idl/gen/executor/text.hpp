// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../executor.hpp"
#include "../out.hpp"
#include "im/ast.hpp"

namespace dci::idl::gen::executor
{
    class Text
        : public Executor
    {
    public:
        Text();
        ~Text() override;

        boost::program_options::options_description options() override;
        bool run(const im::Storage& ims, const boost::program_options::variables_map& vars) override;

    protected:

        virtual void walk(const im::ast::Primitive& v);

        virtual void walk(const im::ast::Array& v);
        virtual void walk(const im::ast::Tuple& v);
        virtual void walk(const im::ast::Ptr& v);
        virtual void walk(const im::ast::Opt& v);

        virtual void walk(const im::ast::List& v);
        virtual void walk(const im::ast::Map& v);
        virtual void walk(const im::ast::Set& v);

        virtual void walk(const im::ast::Alias& v);

        virtual void walk(const im::ast::Enum& v);
        virtual void walk(const im::ast::EnumField& v);

        virtual void walk(const im::ast::Flags& v);
        virtual void walk(const im::ast::FlagsField& v);

        virtual void walk(const im::ast::Scope& v);

        virtual void walk(const im::ast::Variant& v);

        virtual void walk(const im::ast::Exception& v);
        virtual void walk(const im::ast::ExceptionBase& v);
        virtual void walk(const im::ast::ExceptionField& v);

        virtual void walk(const im::ast::Struct& v);
        virtual void walk(const im::ast::StructBase& v);
        virtual void walk(const im::ast::StructField& v);

        virtual void walk(const im::ast::Interface& v);
        virtual void walk(const im::ast::InterfaceBase& v);
        virtual void walk(const im::ast::Method& v);
        virtual void walk(const im::ast::MethodParam& v);

        virtual void walk(const im::ast::TypeUse& v);
        virtual void walk(const im::ast::ScopedName& v);

    protected:
        Out _out;
    };
}

