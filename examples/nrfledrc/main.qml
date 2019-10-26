import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Shapes 1.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.3
import tech.vhrd.ble 1.0

ApplicationWindow {
    id: window
    width: 600
    height: 800
    visible: true
    title: qsTr("RGB RC")
    color: "#121212"

    function checkState() {
        if (uartService.state === UARTService.Ready) {
            stateLabel.text = "Connected"
            stateLabel.color = "#00c853"
        } else {
            if (bleDeviceFinder.state === BLEDeviceFinder.Scanning) {
                stateLabel.text = "Scanning..."
                stateLabel.color = "#7a7cff"
            } else {
                stateLabel.text = "Not connected"
                stateLabel.color = "#ff5131"
            }
        }
    }

    Connections {
        target: uartService
        onStateChanged: {
            checkState()
        }
    }

    Connections {
        target: bleDeviceFinder
        onStateChanged: {
            checkState()
        }
    }

    Component.onCompleted: {
        checkState()
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        interactive: stackView.depth === 1

        ListView {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                highlighted: ListView.isCurrentItem
                onClicked: {
                    listView.currentIndex = index
                    stackView.push(model.source)
                    drawer.close()
                }
            }

            model: ListModel {
                //ListElement { title: "Page"; source: "qrc:/Page.qml" }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }


    header: ToolBar {
        height: 56
        background: Rectangle {
            color: "#202020"
        }

        RowLayout {
            anchors.fill: parent
            spacing: 8

            ToolButton {
                Layout.leftMargin: 8
                icon.name: stackView.depth > 1 ? "back" : "drawer"
                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.pop()
                        listView.currentIndex = -1
                    } else {
                        drawer.open()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: stateLabel
                font.bold: true
            }


            ToolButton {
                text: "Поиск"
                enabled: uartService.state !== UARTService.Ready
                Layout.rightMargin: 8
                onClicked: {
                    bleDeviceFinder.startScan()
                }
            }

//            ToolButton {
//                icon.name: "menu"
//                Layout.rightMargin: 8
//            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: Rectangle {
            id: pane
            color: "#121212"

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 48
                spacing: 24

                PaneWithTitle {
                    title: "RGB Backlight"
                    implicitHeight: parent.height * 0.8

                    Switch {
                        id: mainSwitch
                        anchors.top: parent.top
                        anchors.left: parent.left
                        checked: false
                        onCheckedChanged: ledService.enabled = checked
                    }

                    SpinBox {
                        anchors.top: parent.top
                        anchors.right: parent.right
                        value: 10
                        from: 1
                        to: 100
                        stepSize: 5
                        onValueChanged: ledService.brightness = value * 255 / 100
                        width: 120
                    }

                    GridView {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        height: parent.height * 0.8
                        width: parent.width - 32
                        clip: true
                        anchors.topMargin: 48
                        anchors.leftMargin: (parent.width - Math.floor(width / cellWidth) * cellWidth) / 2
                        cellHeight: 56
                        cellWidth: 56
                        model: ListModel {
                            ListElement { color: "#f44336" }
                            ListElement { color: "red" }
                            ListElement { color: "#6A1B9A" }
                            ListElement { color: "#4527A0" }
                            ListElement { color: "#673AB7" }
                            ListElement { color: "#3F51B5" }
                            ListElement { color: "#2196F3" }
                            ListElement { color: "#03A9F4" }
                            ListElement { color: "#00BCD4" }
                            ListElement { color: "#009688" }
                            ListElement { color: "#4CAF50" }
                            ListElement { color: "#8BC34A" }
                            ListElement { color: "#CDDC39" }
                            ListElement { color: "#FFEB3B" }
                            ListElement { color: "#FFC107" }
                            ListElement { color: "#FF9800" }
                            ListElement { color: "#FF5722" }
                            ListElement { color: "#795548" }
                        }
                        delegate: Rectangle {
                            width: 48
                            height: width
                            radius: width / 2
                            color: model.color
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (mainSwitch.checked) ledService.setColor(model.color)
                            }
                        }
                        ScrollBar.vertical: ScrollBar {

                        }
                    }
                }
            }
        }
    }

//    DebugPane {
//        id: debugPane
//        anchors.fill: parent
//        visible: false
//    }
}
