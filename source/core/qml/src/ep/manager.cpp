// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "manager.hpp"
#include <dci/qml/ep.hpp>

namespace std
{
    template <class Iter>
    Iter begin(const std::pair<Iter, Iter>& range)
    {
        return range.first;
    }

    template <class Iter>
    Iter end(const std::pair<Iter, Iter>& range)
    {
        return range.second;
    }
}

namespace dci::qml::ep
{
    Manager::Manager()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::~Manager()
    {
        if(_app)
        {
            shutdown(_app);
        }

        _extensions.clear();
        _selections.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::registrate(Extension* e, const QString& tag, int priority)
    {
        dbgAssert(e);

        _extensions.insert(ExtState{e, tag, priority});

        if(_app)
        {
            e->startup(_app);
        }

        for(const SelState& ss : _selections.get<1>().equal_range(tag))
        {
            ss._s->update();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::deregistrate(Extension* e)
    {
        auto& idx = _extensions.get<0>();
        auto r = idx.equal_range(e);

        std::set<QString> tagsAffected;

        for(auto iter{r.first}; iter!=r.second; iter = idx.erase(iter))
        {
            tagsAffected.insert(iter->_tag);

            if(_app)
            {
                iter->_e->shutdown(_app);
            }
        }

        for(const QString& tag : tagsAffected)
        {
            for(const SelState& s : _selections.get<1>().equal_range(tag))
            {
                s._s->update();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::deregistrate(Extension* e, const QString& tag)
    {
        auto& idx = _extensions.get<2>();
        auto r = idx.equal_range(std::tuple{e, tag});

        for(auto iter{r.first}; iter!=r.second; iter = idx.erase(iter))
        {
            if(_app)
            {
                iter->_e->shutdown(_app);
            }
        }

        for(const SelState& s : _selections.get<1>().equal_range(tag))
        {
            s._s->update();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::startup(App* app)
    {
        dbgAssert(app);
        dbgAssert(!_app);

        if(_app)
        {
            shutdown(_app);
        }

        dbgAssert(!_app);

        _selections.clear();

        _app = app;

        for(const ExtState& es : _extensions)
        {
            es._e->startup(_app);
        }

        for(const SelState& ss : _selections)
        {
            ss._s->update();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::shutdown(App* app)
    {
        (void)app;

        dbgAssert(_app);
        dbgAssert(_app == app);

        _selections.clear();

        if(_app)
        {
            for(const ExtState& es : _extensions)
            {
                es._e->shutdown(_app);
            }

            _app = nullptr;
        }

        _selections.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SelectionPtr Manager::select(const QString& tag, std::size_t max)
    {
        auto& idx = _selections.get<2>();
        auto iter = idx.find(std::tuple{tag, max});
        if(idx.end() == iter)
        {
            iter = idx.insert(SelState{std::make_shared<Selection>(this, tag, max)}).first;
            iter->_s->update();
        }

        SelectionPtr s = iter->_s;

        return iter->_s;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    App* Manager::app()
    {
        return _app;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QList<Extension *> Manager::getExtensions(const QString& tag, std::size_t max)
    {
        QList<Extension *> res;

        auto& idx = _extensions.get<3>();

        for(
            auto iter = idx.lower_bound(std::tuple{tag, std::numeric_limits<int>::max()});
            iter!=idx.end();
            ++iter)
        {
            const ExtState& es = *iter;
            if(es._tag != tag)
            {
                break;
            }

            res << es._e;
            if(static_cast<std::size_t>(res.size()) >= max)
            {
                break;
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::selectionUninvolved(SelectionPtr s)
    {
        auto& idx = _selections.get<0>();
        idx.erase(s);
        s->detached();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        struct Instance : Manager {} instance;
    }

    Manager& g_manager {instance};
}
