import QtQuick 2.12

MouseArea {
    property string source
    property int imageRotation

    id: root
    hoverEnabled: true

    Rectangle {
        anchors.fill: parent
        color: "green"
        border.color: "yellow"
        border.width: 2
    }

    Image {
        source: root.source
        visible: root.containsMouse
        x: root.mouseX
        y: root.mouseY - height
        fillMode: Image.PreserveAspectFit
        width: 800
        height: 480
        rotation: imageRotation
    }
}
