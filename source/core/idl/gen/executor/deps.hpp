// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "text.hpp"

namespace dci::idl::gen::executor
{
    class Deps
        : public Text
    {
    public:
        Deps();
        ~Deps() override;

        static std::string sname();
        std::string name() override;
        std::string description() override;

        boost::program_options::options_description options() override;

        bool run(const im::Storage& ims, const boost::program_options::variables_map& vars) override;

    private:
        enum class Target
        {
            unknown,
            cpp,
            ninja,
        } _target = Target::unknown;

        std::string _artifact;
    };
}

