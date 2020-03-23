import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    id: root
    clip: true

    property bool revealed: false
    property Item previousFocusItem
    property Item focusItem
    property QtObject event

    onFocusItemChanged: {
        if(previousFocusItem)
            previousFocusItem.highlighted = false;

        previousFocusItem = focusItem;
    }

    onEventChanged: {
        if(event)
        {
            revealed = true;
            focusItem.highlighted = true
            nameField.text = event.name
            catField.text = event.category
            tsField.text = event.timestamp + " s"
            durationField.text = event.duration + " s"

            stats.max = event.max;
            stats.min = event.min;
            stats.average = event.average;
            stats.value = event.duration;

            avgDiff.text = (event.average - event.duration) + " s";
            minmaxDiff.text = (event.max- event.min) + " s";
        }
        else
            revealed = false;
    }

    Row {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.topMargin: 8
        anchors.leftMargin: 8
        spacing: 8
        Column {
            spacing: 8
            Label {
                text: "Name"
                font.bold: true
            }
            Label {
                text: "Category"
                font.bold: true
            }
            Label {
                text: "Timestamp"
                font.bold: true
            }
            Label {
                text: "Duration"
                font.bold: true
            }
        }
        Column {
            spacing: 8
            Label {
                id: nameField
            }
            Label {
                id: catField
            }
            Label {
                id: tsField
            }
            Label {
                id: durationField
            }
        }

        Item {
            id: stats
            width: 300
            height: root.height

            property real average: 0.0
            property real max: 0.0
            property real min: 0.0
            property real value: 0.0

            property int pixelDistance: minLine.y - maxLine.y
            onAverageChanged: {
                avgLine.anchors.bottomMargin = (average - min) * pixelDistance / max;
            }
            onValueChanged: {
                valLine.anchors.bottomMargin = (value - min) * pixelDistance / max;
                console.log(valLine.y);
            }

            Label {
                text: "Timing"
                font.bold: true
                anchors.top: parent.top
                anchors.topMargin: 4
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: "Max"
                anchors.bottom: maxLine.bottom
                anchors.left: parent.left
            }
            Label {
                text: "Min"
                anchors.top: minLine.bottom
                anchors.left: parent.left
            }

            Label {
                text: "Average"
                anchors.top: avgLine.bottom
                anchors.right: parent.right
            }
            Label {
                text: "Current"
                anchors.bottom: valLine.bottom
                anchors.right: parent.right
            }

            Rectangle {
                id: maxLine
                anchors.top: parent.top
                anchors.topMargin: 8 * 3
                width: parent.width
                height: 1
                color: "red"
            }
            Rectangle {
                id: minLine
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8 * 3
                width: parent.width
                height: 1
                color: "orange"
            }
            Rectangle {
                id: avgLine
                anchors.bottom: minLine.bottom
                width: parent.width
                height: 1
                color: "blue"
            }
            Rectangle {
                id: valLine
                anchors.bottom: minLine.bottom
                width: parent.width
                height: 2
                color: "white"
            }
        }
        Column {
            spacing: 8
            Label {
                text: "Min-max difference"
                font.bold: true
            }
            Label {
                text: "Avg difference"
                font.bold: true
            }
        }
        Column {
            spacing: 8
            Label {
                id: minmaxDiff
            }
            Label {
                id: avgDiff
            }
        }
    }
}
