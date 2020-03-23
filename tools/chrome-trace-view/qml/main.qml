import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import me.birchtrees.ctf 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Chrome Trace Viewer")

    header: ToolBar {
        RowLayout {

            ToolButton {
                text: qsTr("Zoom in")
                onClicked: {
                    zoomIn();
                    timelineScale = timelineScale + 1;
                }
            }
            ToolButton {
                text: qsTr("Zoom out")
                onClicked: {
//                    timeView.contentWidth = 0; // TODO: Dirty hack!
                    zoomOut();
                    timelineScale = timelineScale - 1;
                }
            }
        }
    }

    DropArea {
        anchors.fill: parent
        onDropped: {
            if(processes.parsing)
                return;

            console.log("Dropped", drop, drop.proposedAction, drop.text);
            processes.source = decodeURIComponent(drop.text.substr(0, drop.text.length - 1));
            console.log(processes.source);
            dragIndicator.visible = 0.0;
        }
        onEntered: {
            dragIndicator.opacity = 0.5;
        }
        onExited: {
            dragIndicator.visible = 0.0;
        }
    }

    Rectangle {
        id: dragIndicator
        anchors.fill: parent
        color: "black"
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }

        Rectangle {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            anchors.bottomMargin: 8
            anchors.topMargin: 8

            border.width: 4
            border.color: "gray"
            radius: 16
            color: "transparent"

            Label {
                anchors.centerIn: parent
                text: "Visualize file"
                font.pixelSize: 16
                font.bold: true
            }
        }
        z: 2
    }

    Component.onCompleted: {
//        processes.source = "/tmp/Blam Graphics/profile.json"
    }

    TraceModel {
        id: processes

        onTraceParsed: {        
            busy.enabled = false;
            busy.visible = false;

            console.log("Parsed!");
        }

        onTotalDurationChanged: {
            console.log("Duration", totalDuration);
        }

        onParseError: console.log(error)
        onParseUpdate: console.log(message)

        onStartingParse: {
            busy.enabled = true
            busy.visible = true

            console.log("Starting parse")
        }
    }

    Timer {
        id: viewUpdate
        interval: 200
        running: true
        repeat: true
        onTriggered: {
            var oldStart = processes.viewStart;
            var oldEnd = processes.viewEnd;
            processes.viewStart = timeView.contentX * timePerPixel;
            processes.viewEnd = (timeView.contentX + timeView.width) * timePerPixel;
            processes.timePerPixel = timePerPixel

            if(oldStart != processes.viewStart || oldEnd != processes.viewEnd)
            {
                processes.viewUpdated();
            }
        }
    }

    property int rowHeight: 40
    property int headerHeight: 40
    property int threadWidth: 100
    property real timelineScale: 1.0
    property int spacing: 8

    property color threadColor: Qt.lighter(root.color, 1.2)

    // 10^timelineScale is the time over a 100-pixel range
    // Dividing that by 100 we get a good per-pixel time ratio
    // Eg. with a scale of 0 it is 1 second over 100 pixels
    // a scale of 1 gives 1 second over 1000 pixels
    property real timePerPixel: 1 / Math.pow(10, timelineScale + 2)

    signal zoomIn();
    signal zoomOut();

    Rectangle {
        id: busy
        anchors.fill: parent
        color: "#000000"
        opacity: 0.8
        visible: false
        ProgressBar {
            id: busyIndicator
            indeterminate: true
            anchors.centerIn: parent
        }
        Label {
            id: busyText
            text: qsTr("Loading...")
            anchors.top: busyIndicator.bottom
            anchors.horizontalCenter: busy.horizontalCenter

            Connections {
                target: processes
                onParseUpdate: busyText.text = message
                onParseError: busyText.text = error
            }
        }
    }

    footer: InspectorDrawer {
        id: drawer
        color: Qt.darker(root.color, 1.2)
        height: revealed ? 100 : 0
        z: 1
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        color: Qt.darker(root.color, 1.5)
        height: headerHeight
        z: 1

        TimelineBar {
            id: timeline
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: threadWidth

            minSpacing: root.spacing
            visibleWidth: timeView.width
            visibleOffset: timeView.contentX
            timeScale: timelineScale
            timePerPixel: root.timePerPixel
            backgroundColor: Qt.darker(root.color, 1.5)
        }
    }


    Flickable {
        id: timeView
        anchors.fill: parent
        contentWidth: processes.totalDuration * Math.pow(10, timelineScale + 2) + threadWidth + spacing * 4
        contentHeight: headerHeight
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }
        ScrollBar.vertical: ScrollBar {
        }

        Column {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: headerHeight
            spacing: root.spacing
            z: -1

            Repeater {
                model: processes
                Repeater {
                    model: threads
                    ThreadView {
                        onEventClicked: {
                            drawer.focusItem = eventItem;
                            drawer.event = processes.eventFromId(pid, tid, eventId);
                        }

                        Connections {
                            target: root
                            onZoomIn: {
                                updateTimescale(10);
                            }
                            onZoomOut: {
                                updateTimescale(0.1);
                            }
                        }

                        Component.onCompleted: {
                            timeView.contentHeight = timeView.contentHeight +
                                    computedHeight +
                                    root.spacing
                            updateTimescale(1);
                        }

                        width: computedWidth
                        height: computedHeight

                        spacing: root.spacing
                        rowHeight: root.rowHeight
                        threadWidth: root.threadWidth
                        timelineScale: root.timelineScale
                        fillColor: "purple"
                        threadColor: root.threadColor

                        container: timeView

                        viewport: timeView

                        threadName: name
                        maxStackDepth: maxDepth
                        threadEvents: events
                    }
                }
            }
        }
    }
}
