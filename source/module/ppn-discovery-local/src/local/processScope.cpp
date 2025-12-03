// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "processScope.hpp"

namespace dci::module::ppn::discovery::local
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ProcessScope::ProcessScope()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ProcessScope::~ProcessScope()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProcessScope::started()
    {
        for(ProcessScope* other : _randezvous)
        {
            if(other != this)
            {
                other->emitEntries(this->entriesOut());
                emitEntries(other->entriesOut());
            }
        }

        _randezvous.insert(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProcessScope::stopped()
    {
        _randezvous.erase(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ProcessScope::declared(const Entry& entry)
    {
        for(ProcessScope* other : _randezvous)
        {
            if(other != this)
            {
                other->emitEntry(entry);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ProcessScope::Randezvous ProcessScope::_randezvous;
}
