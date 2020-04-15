import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

RowLayout {
    id: root

    property int minSpacing: 8
    property int visibleWidth: 640
    property int visibleOffset: 0
    property int timeScale: 1.0
    property real timePerPixel
    property color backgroundColor

    QtObject {
        id: p
        property int wrapAlignment: visibleWidth % 100
        property int adjustedVisibleWidth: visibleWidth - wrapAlignment

        property int currentPage: (visibleOffset / p.adjustedVisibleWidth)

        property string suffix: "s"
    }

    onTimeScaleChanged: {
        return;
        if(timeScale <= 1)
            p.suffix = "s";
        else if(timeScale <= 3)
            p.suffix = "ms";
        else if(timeScale >= 6)
            p.suffix = "us";
    }

    spacing: minSpacing

    Item {
        height: headerHeight
        width: root.width

        /* Second ticks with text */
        Item {
            id: secondTicks
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: minSpacing
            height: (root.height / 3) * 2

            Repeater {
                model: root.width * 2 / 100
                Label {
                    x: index * 100 * (Screen.pixelDensity / 4) -
                            width / 2 -
                            visibleOffset % p.adjustedVisibleWidth
                    text: (index * 100 * timePerPixel + p.adjustedVisibleWidth * p.currentPage * timePerPixel) * (Screen.pixelDensity / 4) + p.suffix
                    clip: true
                }
            }
        }
        /* Sub-second ticks */
        Item {
            anchors.top: secondTicks.bottom
            anchors.left: parent.left
            anchors.leftMargin: minSpacing
            height: root.height / 3
            Repeater {
                model: root.width * 2 / 10
                Rectangle {
                    x: index * 10 * (Screen.pixelDensity / 4) -
                            visibleOffset % p.adjustedVisibleWidth
                    width: 2 * (Screen.pixelDensity / 4)
                    height: (index % 10 == 0) ? parent.height * 1.2 : parent.height
                    color: (index % 10 == 0) ? "white" : "#555"
                }
            }
        }
    }
}
