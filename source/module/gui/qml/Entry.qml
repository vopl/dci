import QtQuick
import Qt.labs.platform
import "." as Near

QtObject
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    property SystemTrayIcon trayIcon: SystemTrayIcon {
        icon.source: Qt.resolvedUrl("icon.svg");
        visible: true

        menu: Menu {
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.exit(0)
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    property Near.Window window: Near.Window {}
}
