import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Drawer {
    id: root

    property bool revealed: false
    property Item previousFocusItem
    property Item focusItem
    property QtObject event
    property Item drawerBackground

    background: drawerBackground

    onFocusItemChanged: {
        if(previousFocusItem)
            previousFocusItem.highlighted = false;

        previousFocusItem = focusItem;
    }

    onEventChanged: {
        if(event)
        {
            revealed = true;
            focusItem.highlighted = true;

            nameField.value = event.name
            catField.value = event.category
            tsField.value = event.timestamp.toFixed(5) + " s"
            durationField.value = event.duration.toFixed(5) + " s"

            stats.max = event.max.toFixed(5);
            stats.min = event.min.toFixed(5);
            stats.average = event.average.toFixed(5);
            stats.value = event.duration.toFixed(5);

            avgDiff.value = (event.average - event.duration).toFixed(5) + " s";
            minmaxDiff.value = (event.max- event.min).toFixed(5) + " s";

            totalField.value = event.numEvents;
            avgValue.value = event.average.toFixed(5) + " s";

            open();
        }
        else
        {
            revealed = false;
            close();
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 2 * Screen.pixelDensity
        anchors.leftMargin: 2 * Screen.pixelDensity
        spacing: 2 * Screen.pixelDensity
        height: parent.height

        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: 20 * Screen.pixelDensity
            spacing: 2 * Screen.pixelDensity

            KeyValueProperty {
                Layout.fillWidth: true
                id: nameField
                name: "Name"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: catField
                name: "Category"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: tsField
                name: "Timestamp"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: durationField
                name: "Duration"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: totalField
                name: "Num. events"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: minmaxDiff
                name: "Min-max difference"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: avgDiff
                name: "Avg difference"
            }
            KeyValueProperty {
                Layout.fillWidth: true
                id: avgValue
                name: "Average"
            }
        }

        Item {
            Layout.fillWidth: true
            id: stats
            Layout.minimumWidth: 20 * Screen.pixelDensity
            height: parent.height

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
            }

            Label {
                text: "Timing"
                color: "white"
                font.bold: true
                anchors.top: parent.top
                anchors.topMargin: Screen.pixelDensity
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: "Max"
                color: "white"
                anchors.bottom: maxLine.bottom
                anchors.left: parent.left
            }
            Label {
                text: "Min"
                color: "white"
                anchors.top: minLine.bottom
                anchors.left: parent.left
            }

            Label {
                text: "Average"
                color: "white"
                anchors.top: avgLine.bottom
                anchors.right: parent.right
            }
            Label {
                text: "Current"
                color: "white"
                anchors.bottom: valLine.bottom
                anchors.right: parent.right
            }

            Rectangle {
                id: maxLine
                anchors.top: parent.top
                anchors.topMargin: 2 * 3 * Screen.pixelDensity
                width: parent.width
                height: Screen.pixelDensity / 4
                color: "red"
            }
            Rectangle {
                id: minLine
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 2 * 3 * Screen.pixelDensity
                width: parent.width
                height: Screen.pixelDensity / 4
                color: "orange"
            }
            Rectangle {
                id: avgLine
                anchors.bottom: minLine.bottom
                width: parent.width
                height: Screen.pixelDensity / 4
                color: "blue"
            }
            Rectangle {
                id: valLine
                anchors.bottom: minLine.bottom
                width: parent.width
                height: Screen.pixelDensity / 2
                color: "white"
            }
        }
    }
}
