// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/qml/qmeta.hpp>
#include <dci/qml/app.hpp>


#include <QObject>

struct Maker
{
    Q_GADGET

    int l;
};

using namespace dci;
using namespace dci::idl;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(qml, qmeta_array)
{
    qml::AppPtr app = qml::App::instance();
    app->qengine();
}
