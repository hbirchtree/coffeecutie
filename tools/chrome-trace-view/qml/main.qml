import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12

//import dev.birchy.ctf 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 1200

    color: "#111"

    title: qsTr("Chrome Trace Viewer")

    property int rowHeight: 10 * Screen.pixelDensity
    property int headerHeight: 10 * Screen.pixelDensity
    property int threadWidth: 25 * Screen.pixelDensity
    property real timelineScale: 1.0
    property int spacing: 2 * Screen.pixelDensity

    property color threadColor: Qt.lighter(root.color, 1.2)

    property color subWindowBackground: "#222"

    // 10^timelineScale is the time over a 100-pixel range
    // Dividing that by 100 we get a good per-pixel time ratio
    // Eg. with a scale of 0 it is 1 second over 100 pixels
    // a scale of 1 gives 1 second over 1000 pixels
    property real timePerPixel: 1 / Math.pow(10, timelineScale + 2) / (Screen.pixelDensity / 4)

    signal zoomIn();
    signal zoomOut();

    header: ToolBar {
        background: Rectangle { color: subWindowBackground }
        height: Screen.pixelDensity * 10
        RowLayout {
            spacing: 0
            anchors.fill: parent
            ToolButton {
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Open file")
                onClicked: {
                    processes.openFile();
                }
            }
            ToolButton {
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Zoom in")
                onClicked: {
                    zoomIn();
                    timelineScale = timelineScale + 1;
                }
            }
            ToolButton {
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Zoom out")
                onClicked: {
                    zoomOut();
                    timelineScale = timelineScale - 1;
                }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                Layout.alignment: Qt.AlignRight
                text: qsTr("More Info")
                onClicked: {
                    extraInfoDrawer.open();
                }
            }
        }
    }

    InspectorDrawer {
        id: drawer
        edge: Qt.BottomEdge
        width: root.width
        height: threadWidth * 1.8
        modal: false
        interactive: false
        drawerBackground: Rectangle { color: subWindowBackground }
    }

    Component.onCompleted: {
        if(isEmscripten)
            processes.emscriptenAuto();
        else
            processes.filesystemAuto();
    }

    DropArea {
        anchors.fill: parent
        onDropped: {
            dragIndicator.close();

            if(processes.parsing)
                return;

            console.log("Dropped", drop, drop.proposedAction, drop.text);
            processes.source = decodeURIComponent(drop.text.substr(0, drop.text.length - 1));
            console.log(processes.source);
        }
        onEntered: {
            dragIndicator.open();
        }
        onExited: {
            dragIndicator.close();
        }
    }

    Popup {
        id: dragIndicator
        modal: true
        closePolicy: Popup.NoAutoClose
        parent: Overlay.overlay
        anchors.centerIn: Overlay.overlay

        background: Rectangle {
            anchors.centerIn: parent
            border.width: root.spacing / 2
            border.color: "gray"
            radius: root.spacing * 2
            color: "transparent"
            width: Math.min(threadWidth * 2, root.width)
            height: Math.min(threadWidth * 2, root.height)

            Label {
                anchors.centerIn: parent
                text: "Visualize file"
                font.pixelSize: root.spacing * 2
                font.bold: true
            }
        }
    }

    Connections {
        target: processes

        function onTraceParsed() {
            busy.close();
            console.log("Parsed!");
        }

        function onTotalDurationChanged(totalDuration) {
            console.log("Duration", totalDuration);
        }

        function onParseError(error) {
            console.log(error);
        }
        function onParseUpdate(message) {
            console.log(message);
        }

        function onStartingParse(ss) {
            busy.open();
            console.log("Starting parse");
        }
    }

    Popup {
        id: busy
        modal: true
        closePolicy: Popup.NoAutoClose
        parent: Overlay.overlay
        anchors.centerIn: Overlay.overlay

        Column {
            ProgressBar {
                id: busyIndicator
                indeterminate: true
            }
            Label {
                id: busyText
                text: qsTr("Loading...")

                Connections {
                    target: processes
                    function onParseUpdate(message) {
                        busyText.text = message;
                    }
                    function onParseError(error) {
                        busyText.text = error;
                    }
                }
            }
        }
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

    Drawer {
        id: extraInfoDrawer
        width: threadWidth * 4
        height: root.height
        edge: Qt.RightEdge
        background: Rectangle { color: subWindowBackground }

        ScrollView {
            anchors.fill: parent
            anchors.leftMargin: root.spacing
            anchors.topMargin: root.spacing
            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: root.spacing
                Repeater {
                    model: processes.extraInfo
                    GroupBox {
                        title: groupName
                        width: parent.width
                        Layout.fillWidth: true
                        ColumnLayout {
                            width: parent.width
                            Repeater {
                                model: group
                                KeyValueProperty {
                                    name: infoKey
                                    value: infoValue
                                    Layout.maximumWidth: threadWidth * 4 - root.spacing * 6
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Flickable {
        id: timeView
        anchors.fill: parent
        anchors.bottomMargin: drawer.opened ? drawer.height : 0
        contentWidth: processes.totalDuration * Math.pow(10, timelineScale + 2) * (Screen.pixelDensity / 4) + threadWidth + spacing * 4
        contentHeight: headerHeight + timelineColumn.height
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }

        ColumnLayout {
            id: timelineColumn
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
                            function onZoomIn() {
                                updateTimescale(10);
                            }
                            function onZoomOut() {
                                updateTimescale(0.1);
                            }
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

            Repeater {
                model: processes.metrics

                MetricView {
                    width: timeView.contentWidth
                    height: root.rowHeight * 2

                    source: metric

                    container: timeView
                    timePerPixel: root.timePerPixel

                    threadColor: root.threadColor
                    rowHeight: root.rowHeight
                    threadWidth: root.threadWidth
                    spacing: root.spacing

                    onClicked: {
                        drawer.focusItem = item;
                        drawer.event = model;
                    }
                    Component.onCompleted: console.log('metric', x, y, height)
                }
            }
        }
    }
}
