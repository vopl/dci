// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "lookout.hpp"
#include "performer.hpp"
#include "../../mapper.hpp"

namespace dci::module::ppn::transport::natt::mapper::custom
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::Lookout(Mapper* mapper)
        : mapper::Lookout{mapper}
    {
        _name = "custom";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::~Lookout()
    {
        deactivate(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::map(const apit::Address& i, const apit::Address& e)
    {
        _maps.emplace(i, e);
        ++_revision;
        _changed.raise();
        if(!activate(this))
        {
            mapper()->lookoutChanged(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::unmap(const apit::Address& i, const apit::Address& e)
    {
        bool unmapped = false;

        auto iter = _maps.find(i);
        while(_maps.end() != iter && i == iter->first)
        {
            if(e == iter->second)
            {
                _maps.erase(iter);
                ++_revision;
                _changed.raise();
                unmapped = true;
                break;
            }
        }

        if(unmapped)
        {
            if(_maps.empty())
            {
                deactivate(this);
            }
            else
            {
                mapper()->lookoutChanged(this);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PerformerBlank Lookout::candidate(const apit::Address& internal)
    {
        if(_maps.contains(internal))
        {
            return PerformerBlank
            {
                PerformerBlank::Builder
                {
                    [=,this]{return PerformerPtr{new Performer{this, internal}, [](mapper::Performer*p){delete static_cast<Performer*>(p);}};}
                },
                100
            };
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Lookout::Maps& Lookout::maps() const
    {
        return _maps;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Lookout::revision() const
    {
        return _revision;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Waitable& Lookout::changed()
    {
        return _changed;
    }

}
