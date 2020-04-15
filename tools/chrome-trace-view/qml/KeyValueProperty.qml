import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

RowLayout {
    property string name
    property string value

    property Label valueItem: valueField

    Label {
        id: nameField
        text: name
        font.bold: true
    }
    Item {
        Layout.fillWidth: true
    }
    Label {
        Layout.alignment: Qt.AlignRight
        Layout.maximumWidth: parent.width - nameField.width
        id: valueField
        text: value
    }
}
