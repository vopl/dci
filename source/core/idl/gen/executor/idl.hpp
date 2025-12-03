// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "text.hpp"

namespace dci::idl::gen::executor
{
    class Idl
        : public Text
    {
    public:
        Idl();
        ~Idl() override;

        static std::string sname();
        std::string name() override;
        std::string description() override;

        boost::program_options::options_description options() override;

        bool run(const im::Storage& ims, const boost::program_options::variables_map& vars) override;

    private:
        void walk(const im::ast::Primitive& v) override;

        void walk(const im::ast::Array& v) override;
        void walk(const im::ast::Tuple& v) override;
        void walk(const im::ast::Ptr& v) override;
        void walk(const im::ast::Opt& v) override;

        void walk(const im::ast::List& v) override;
        void walk(const im::ast::Map& v) override;
        void walk(const im::ast::Set& v) override;

        void walk(const im::ast::Alias& v) override;

        void walk(const im::ast::Enum& v) override;
        void walk(const im::ast::EnumField& v) override;

        void walk(const im::ast::Flags& v) override;
        void walk(const im::ast::FlagsField& v) override;

        void walk(const im::ast::Scope& v) override;

        void walk(const im::ast::Variant& v) override;

        void walk(const im::ast::Exception& v) override;
        void walk(const im::ast::ExceptionBase& v) override;
        void walk(const im::ast::ExceptionField& v) override;

        void walk(const im::ast::Struct& v) override;
        void walk(const im::ast::StructBase& v) override;
        void walk(const im::ast::StructField& v) override;

        void walk(const im::ast::Interface& v) override;
        void walk(const im::ast::InterfaceBase& v) override;
        void walk(const im::ast::Method& v) override;
        void walk(const im::ast::MethodParam& v) override;

        void walk(const im::ast::TypeUse& v) override;
        void walk(const im::ast::ScopedName& v) override;

    private:
        template <class T>
        void writeSign(const T& v);

    private:
        bool _writeSigns{false};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Idl::writeSign(const T& v)
    {
        if(!_writeSigns)
        {
            return;
        }

        if constexpr(requires{v->sign4Name;})
            _out<<"//sign4Name: "<<v->sign4Name.toHex()<<el;
        if constexpr(requires{v->sign4Layout;})
            _out<<"//sign4Layout: "<<v->sign4Layout.toHex()<<el;
    }
}

