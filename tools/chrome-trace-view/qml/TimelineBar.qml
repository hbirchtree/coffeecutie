import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {
    id: root

    property int minSpacing: 8
    property int contentWidth: 100
    property int timeScale: 1.0
    property color backgroundColor

    property string suffix: "s"
    property real tickInterval: 10.0
    property real timeMultiplier: 1

    onTimeScaleChanged: {
        tickInterval = 20 / timeScale;

        // Zoom level 2: 10 tickers == 1 second
        // Zoom level 4: 10 tickers == 1/4 second
        // Zoom level 8: 10 tickers == 1/8 second

        if(timeScale <= 2)
            suffix = "s";
        else if(timeScale <= 4)
            suffix = "ms";
         else if(timeScale <= 8)
            suffix = "ms";
        else if(timeScale <= 16)
            suffix = "ms";
        else if(timeScale <= 32)
            suffix = "us";

        if(suffix == "s")
            timeMultiplier = 1;
        if(suffix == "ms")
            timeMultiplier = 1000;
        else if(suffix == "us")
            timeMultiplier = 1000000;
    }

    spacing: minSpacing

    Rectangle {
        color: backgroundColor
        height: headerHeight
        width: contentWidth

        /* Second ticks with text */
        Item {
            id: secondTicks
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: minSpacing
            height: (root.height / 3) * 2

            Repeater {
                model: contentWidth / (timeScale * tickInterval)
                Label {
                    x: index * tickInterval * timeScale * 10 - width / 2
                    text: index * timeMultiplier / (timeScale / 2) + suffix
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
                model: contentWidth / (timeScale * tickInterval)
                Rectangle {
                    x: index * tickInterval * timeScale
                    width: 2
                    height: parent.height
                    color: (index % tickInterval == 0) ? "white" : "#555"
                }
            }
        }
    }
}
