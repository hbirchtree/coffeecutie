import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root

    property int spacing: 8
    property int rowHeight: 20
    property int threadWidth: 40
    property real timelineScale: 1.0

    property color threadColor: "black"
    property color fillColor: "blue"

    // For aligning the description
    property Flickable container

    // Thread properties
    property string threadName
    property int maxStackDepth: 1
    property QtObject threadEvents

    // Computed for external use
    property real eventsComputedWidth: 0
    property real computedWidth: eventsComputedWidth + threadWidth + spacing
    property real computedHeight: Math.max(maxStackDepth * rowHeight, rowHeight)

    property var viewport

    signal eventClicked(Item eventItem, var pid, var tid, int eventId)
    signal timescaleUpdate(int computedWidth)

    function updateTimescale(scale) {
        eventsComputedWidth = eventsComputedWidth * scale;
        computedWidth = eventsComputedWidth + threadWidth + spacing;

        timescaleUpdate(computedWidth);
    }

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

    Item {
        clip: true
        id: eventContainer
        height: computedHeight
        width: eventsComputedWidth
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: threadWidth + spacing
        z: -1

        Repeater {
            model: threadEvents
            Scope {
                property bool highlighted: false
                property color baseColor: fillColor

                color: Qt.lighter(baseColor, highlighted ? 1.2 : 1)
                x: ts * Math.pow(10, timelineScale + 2)
                y: stackDepth * rowHeight
                width: time * Math.pow(10, timelineScale + 2)
                height: rowHeight
                text: name ? name : "Unknown event"
                container: root.container

                onClicked: {
                    root.eventClicked(this, pid, tid, eventId);
                }

                Component.onCompleted: {
                    eventsComputedWidth = Math.max(eventsComputedWidth, x + width)
                }
            }
        }
    }
}
