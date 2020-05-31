import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Item {
    id: root

    property int spacing: 2 * Screen.pixelDensity
    property int rowHeight: 5 * Screen.pixelDensity
    property int threadWidth: 10 * Screen.pixelDensity
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
        clip: true

        Label {
            anchors.fill: parent
            anchors.leftMargin: spacing
            anchors.topMargin: spacing
            text: name ? name : "Thread"
            color: "white"
            wrapMode: Label.WordWrap
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
                x: ts * Math.pow(10, timelineScale + 2) * (Screen.pixelDensity / 4)
                y: stackDepth * rowHeight
                width: time * Math.pow(10, timelineScale + 2) * (Screen.pixelDensity / 4)
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
