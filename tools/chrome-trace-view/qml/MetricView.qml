import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Shapes 1.12

Item {
    id: root

    property var source

    property color threadColor
    property int spacing
    property int rowHeight
    property int threadWidth
    property Item container

    property real timePerPixel

    property real metricHeight: height - spacing * 2

    Rectangle {
        width: threadWidth
        height: root.height
        anchors.left: root.left
        anchors.top: root.top
        anchors.leftMargin: container.contentX
        color: threadColor
        Label {
            text: metricName
        }
    }

    Repeater {
        model: metric
        Shape {
            z: -1
            anchors.top: root.top
            anchors.bottom: root.bottom
            anchors.left: root.left
            anchors.right: root.right
            anchors.topMargin: spacing
            anchors.bottomMargin: spacing
            anchors.leftMargin: threadWidth

            clip: true

            property real vertX1: previousTs / timePerPixel
            property real vertX2: ts / timePerPixel

            property real vertY1: (1.0 - previousValueScaled) * metricHeight * 0.8 + 0.2
            property real vertY2: (1.0 - valueScaled) * metricHeight * 0.8 + 0.2

            ShapePath {
                strokeColor: "transparent"
                fillColor: "#005000"
                fillRule: ShapePath.WindingFill

                startX: vertX1
                startY: vertY1
                PathLine {
                    x: vertX2
                    y: vertY2
                }
                PathLine {
                    x: vertX2
                    y: metricHeight
                }
                PathLine {
                    x: vertX1
                    y: metricHeight
                }
                PathLine {
                    x: vertX1
                    y: vertY1
                }
            }
            ShapePath {
                strokeColor: "green"
                strokeWidth: 2
                fillColor: "transparent"
                capStyle: ShapePath.RoundCap

                startX: vertX1
                startY: vertY1
                PathLine {
                    x: vertX2
                    y: vertY2
                }
            }
        }
    }

    Rectangle {
        id: markerLine
        color: "#500"
        anchors.top: root.top
        anchors.topMargin: spacing
        width: 2
        height: metricHeight
        z: -1
        visible: false

        Label {
            id: markerText
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPositionChanged: {
            markerText.text = metric.sampleValue((mouse.x - threadWidth) * timePerPixel) + metric.unit;
            markerLine.x = mouse.x;
        }
        onEntered: markerLine.visible = true
        onExited: markerLine.visible = false
    }
}
