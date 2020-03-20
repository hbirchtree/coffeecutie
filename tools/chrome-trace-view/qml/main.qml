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
                onClicked: timelineScale *= 2
            }
            ToolButton {
                text: qsTr("Zoom out")
                onClicked: {
                    timelineScale = Math.max(timelineScale / 2, 1)
                }
            }
        }
    }

    Component.onCompleted: {
        processes.source = "/tmp/GLeam Basic RHI/profile.json"
    }

    TraceModel {
        id: processes
        onTraceParsed: {        
            busy.enabled = false
            busy.visible = false

            console.log("Parsed!")
        }
        onParseError: console.log(error)
        onParseUpdate: console.log(message)

        onStartingParse: {
            busy.enabled = true
            busy.visible = true

            console.log("Starting parse")
        }
    }

    property int rowHeight: 40
    property int headerHeight: 40
    property int threadWidth: 100
    property real timelineScale: 2.0
    property int spacing: 8

    property color threadColor: Qt.lighter(root.color, 1.2)

    Rectangle {
        id: busy
        anchors.fill: parent
        color: "#000000"
        opacity: 0.8
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
            }
        }
    }

    Flickable {
        id: timeView
        anchors.fill: parent
        contentWidth: 0
        contentHeight: headerHeight * 2
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }
        ScrollBar.vertical: ScrollBar {

        }

        TimelineBar {
            id: timeline
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: threadWidth

            minSpacing: root.spacing
            contentWidth: width
            timeScale: timelineScale
            backgroundColor: Qt.darker(root.color, 1.5)

            width: eventColumn.width
            height: headerHeight
        }

        Column {
            id: eventColumn
            clip: true
            anchors.left: parent.left
            anchors.top: timeline.bottom
            spacing: root.spacing
            z: -1

            Repeater {
                model: processes
                Repeater {
                    model: threads
                    ThreadView {
                        Component.onCompleted: {
                            console.log(computedWidth, computedHeight)
                            timeView.contentHeight = timeView.contentHeight +
                                    computedHeight +
                                    root.spacing
                            timeView.contentWidth = timeView.contentWidth +
                                    computedWidth
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

                        name: name
                        maxStackDepth: maxDepth
                        threadEvents: events
                    }
                }
            }
        }
    }
}
