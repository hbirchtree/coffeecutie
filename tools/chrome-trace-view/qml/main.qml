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
        processes.source = "/tmp/Blam Graphics/profile.json"
    }

    TraceModel {
        id: processes
        onTraceParsed: {        
            busy.enabled = false
            busy.visible = false

            console.log("Parsed!")
        }
        onParseError: {
            console.log(error);
        }

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

//    Rectangle {
//        id: busy
//        anchors.fill: parent
//        color: "#000000f0"
//        BusyIndicator {
//            anchors.centerIn: parent
//        }
//    }


    ScrollView {
        anchors.fill: parent
        clip: true

        Label {
            text: "hello"
            font.pixelSize: 300
            anchors.bottom: parent.bottom
        }

//        Column {
//            clip: true
//            id: threadsBar
//            width: threadWidth
//            spacing: 8

//            Item {
//                height: headerHeight
//                width: parent.width
//            }

//            Repeater {
//                model: processes

//                Repeater {
//                    model: threads

//                    Rectangle {
//                        color: Qt.lighter(root.color, 1.5)
//                        width: threadWidth
//                        height: rowHeight + (maxDepth - 1) * 20

//                        Component.onCompleted: console.log(name)

//                        Label {
//                            text: name
//                        }

//                        Label {
//                            text: "Async"
//                            anchors.bottom: parent.bottom
//                        }
//                    }
//                }
//            }
//        }

////        Rectangle {
////            anchors.left: threadsBar.right
////            anchors.top: timeline.bottom
////            anchors.leftMargin: 8
////            anchors.topMargin: 8
////            height: 20
////            width: 2000
////            color: "red"
////        }

//        TimelineBar {
//            id: timeline
//            anchors.left: threadsBar.right
//            anchors.top: parent.top
//            anchors.leftMargin: 8

//            minSpacing: 8
//            contentWidth: 2000
//            timeScale: timelineScale
//            backgroundColor: Qt.darker(root.color, 1.5)

//            width: 2000
//            height: headerHeight
//        }

//        Column {
//            clip: false
//            anchors.left: threadsBar.right
//            anchors.top: timeline.bottom
//            anchors.leftMargin: 8
//            anchors.topMargin: 8
//            spacing: 8

//            Repeater {
//                model: processes
//                Repeater {
//                    model: threads
//                    Rectangle {
//                        height: rowHeight + (maxDepth - 1) * 20
//                        width: 2000
//                        color: "purple"

//                        Repeater {
//                            model: events
//                            Scope {
//                                color: Qt.darker(fillColor, focus ? 50 : 1)
//                                x: ts * 100 * timelineScale
//                                y: stackDepth * height
//                                width: 10 * 100 * timelineScale
//                                height: 20
//                                text: name
//                            }
//                        }
//                    }
//                }
//            }
//        }
    }
}
