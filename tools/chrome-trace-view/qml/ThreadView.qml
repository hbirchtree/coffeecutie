import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    clip: true

    property int spacing: 8
    property int rowHeight: 20
    property int threadWidth: 40
    property real timelineScale: 1.0

    property color threadColor: "black"
    property color fillColor: "blue"

    // For aligning the description
    property Flickable container

    // Thread properties
    property string name
    property int maxStackDepth: 1
    property QtObject threadEvents

    // Computed for external use
    property real eventsComputedWidth: 0
    property real computedWidth: eventsComputedWidth + threadWidth + spacing
    property real computedHeight: Math.max(maxStackDepth * rowHeight, rowHeight)

    Rectangle {
        width: threadWidth
        height: computedHeight
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: container.contentX
        color: threadColor

        Label {
            text: name ? name : "Thread"
            anchors.leftMargin: spacing
            anchors.topMargin: spacing
            anchors.fill: parent
        }
    }

    Rectangle {
        id: eventContainer
        height: computedHeight
        width: eventsComputedWidth
        color: "purple"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: threadWidth + spacing
        z: -1

        Repeater {
            model: threadEvents
            Scope {
                color: Qt.darker(fillColor, focus ? 50 : 1)
                x: ts * 100 * timelineScale
                y: stackDepth * rowHeight
                width: 10 * 100 * timelineScale
                height: rowHeight
                text: name

                Component.onCompleted: {
                    eventsComputedWidth = Math.max(eventsComputedWidth, x + width)
                    console.log("Event time:", ts, "+", time)
                }
            }
        }
    }
}
