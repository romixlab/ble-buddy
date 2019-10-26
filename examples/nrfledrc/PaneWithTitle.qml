import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.3

Pane {
    property alias title: titleText.text
    Layout.fillWidth: true
    Layout.margins: 16
    Material.elevation: 8
    Text {
        id: titleText
        color: "#a4a4a4"
        anchors.bottom: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.bottomMargin: 24
        font.bold: true
        font.pointSize: 18
    }
}
