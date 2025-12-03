// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include <dci/qml/ep.hpp>
#include "ep/manager.hpp"

namespace dci::qml::ep
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Extension::~Extension()
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Extension::startup(App *)
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Extension::shutdown(App *)
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* Extension::activate(App *, const QString&)
    {
        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void registrate(Extension* e, const QString& tag, int priority)
    {
        g_manager.registrate(e, tag, priority);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void deregistrate(Extension* e)
    {
        g_manager.deregistrate(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void deregistrate(Extension* e, const QString& tag)
    {
        g_manager.deregistrate(e, tag);
    }
}
