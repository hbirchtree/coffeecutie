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
    property bool highlighted: false

    property real timePerPixel

    property real metricHeight: height - spacing * 2

    signal clicked(var model, Item item)

    Rectangle {
        width: threadWidth
        height: root.height
        anchors.left: root.left
        anchors.top: root.top
        anchors.leftMargin: container.contentX
        color: threadColor
        Label {
            anchors.fill: parent
            text: metricName
            color: "white"
            wrapMode: Label.WordWrap
        }
    }

    onSourceChanged: {
        if(metric.type == 0)
            valueLoader.active = true;
        else if(metric.type == 2)
            markerLoader.active = true;
    }

    Loader {
        id: valueLoader
        active: false
        anchors.fill: parent
        z: -1
        sourceComponent: Repeater {
            model: metric
            Shape {
                anchors.fill: parent
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
                    strokeWidth: highlighted ? 4 : 2
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
    }
    Loader {
        id: markerLoader
        active: false
        anchors.fill: parent
        z: -1
        opacity: 0.2

        sourceComponent: Repeater {
            model: metric
            Shape {
                anchors.fill: parent
                anchors.topMargin: spacing
                anchors.leftMargin: threadWidth

                ShapePath {
                    strokeColor: "yellow"
                    strokeWidth: highlighted ? 2 : 1
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap

                    startX: ts / timePerPixel
                    startY: metricHeight

                    PathLine {
                        x: ts / timePerPixel
                        y: -container.contentHeight
                    }
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
        enabled: valueLoader.active

        Label {
            id: markerText
            color: "white"
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: valueLoader.active
        onPositionChanged: {
            if(!hoverEnabled)
                return;

            markerText.text = (metric.sampleValue((mouse.x - threadWidth) * timePerPixel)).toFixed(4) + metric.unit;
            markerLine.x = mouse.x;
        }
        onClicked: {
            root.clicked(source, root);
        }
        onEntered: markerLine.visible = true
        onExited: markerLine.visible = false
    }
}
