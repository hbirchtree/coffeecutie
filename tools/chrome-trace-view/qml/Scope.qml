import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    clip: true

    property string text
    property int identifier
    property Flickable container

    signal clicked()

    Label {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 8 + Math.max(0, container.contentX - root.x)
        text: parent.width < 40 ? "" : root.text
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
