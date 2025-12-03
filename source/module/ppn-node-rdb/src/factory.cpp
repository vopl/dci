// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "factory.hpp"
#include "instance.hpp"

namespace dci::module::ppn::node::rdb
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Factory::Factory()
        : api::Factory<>::Opposite(idl::interface::Initializer())
    {
        //in build(list<Feature> features) -> Instance;
        methods()->build() += serviceSol() * [this](List<api::Feature<>>&& features)
        {
            Instance* instance = new Instance;
            instance->involvedChanged() += instance * [instance](bool v)
            {
                if(!v)
                {
                    delete instance;
                }
            };

            return instance->initialize(std::move(features)).chain<api::Instance<>>(serviceSol(), [res=instance->opposite()](auto in, auto& out)
            {
                if(in.resolvedCancel())
                {
                    out.resolveCancel();
                }
                else if(in.resolvedException())
                {
                    out.resolveException(in.detachException());
                }
                else
                {
                    out.resolveValue(res);
                }
            });
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Factory::~Factory()
    {
        serviceSol().flush();
    }
}
