import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    property string text

    Label {
        anchors.verticalCenter: parent.verticalCenter
        text: parent.width < 20 ? "" : root.text
    }
}
