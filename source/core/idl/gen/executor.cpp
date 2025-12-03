// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "executor.hpp"
#include "executor/idl.hpp"
#include "executor/cpp.hpp"
#include "executor/deps.hpp"

namespace dci::idl::gen
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        class ExecutorsMap
            : public std::map<std::string, std::function<ExecutorPtr()>>
        {
        public:
            ExecutorsMap()
            {
                insert(std::make_pair(executor::Idl::sname(), [](){return ExecutorPtr(new executor::Idl);}));
                insert(std::make_pair(executor::Cpp::sname(), [](){return ExecutorPtr(new executor::Cpp);}));
                insert(std::make_pair(executor::Deps::sname(), [](){return ExecutorPtr(new executor::Deps);}));
            }
        } executorsMap;
    }

    const std::map<std::string, std::function<ExecutorPtr()>>& Executor::getAll()
    {
        return executorsMap;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Executor::Executor()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Executor::~Executor()
    {

    }
}
