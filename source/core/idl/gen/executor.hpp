// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <boost/program_options.hpp>
#include "im/storage.hpp"

namespace dci::idl::gen
{
    struct Executor;
    using ExecutorPtr = std::shared_ptr<Executor>;

    struct Executor
    {
        static const std::map<std::string, std::function<ExecutorPtr()>>& getAll();

        Executor();
        virtual ~Executor();

        virtual std::string name() = 0;
        virtual std::string description() = 0;

        virtual boost::program_options::options_description options() = 0;

        virtual bool run(const im::Storage& ims, const boost::program_options::variables_map& vars) = 0;
    };
}
