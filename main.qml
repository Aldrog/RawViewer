import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import cvs.rawviewer 1.0

ApplicationWindow {
    id: appWindow

    function isDarkColor(color) {
        color = Qt.darker(color, 1)

        var a = 1 - (0.299 * color.r + 0.587 * color.g + 0.114 * color.b)

        return color.a > 0 && a >= 0.3
    }

    function lightDark(background, lightColor, darkColor) {
        return isDarkColor(background) ? darkColor : lightColor
    }

    visible: true
    width: (settings.viewMode === ViewSettings.Station ? 918 : 1224)
    height: (settings.viewMode === ViewSettings.Station ? 512 : 1024) + appBar.height
    title: qsTr("Hello LPS")

    Material.primary: Material.Blue
    Material.accent: Material.Red

    header: ToolBar {
        id: appBar
        Material.background: Material.primaryColor
        Material.theme: lightDark(Material.background, Material.Light, Material.Dark)

        RowLayout {
            anchors {
                fill: parent
                rightMargin: 16
                leftMargin: 16
            }

            Label {
                text: fileName
                elide: Label.ElideRight
                font.pixelSize: 20
                font.weight: Font.Medium
                color: Material.primaryTextColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                Layout.alignment: Qt.AlignLeft
            }

            ToolButton {
                text: qsTr("›‹")
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    image.scale = 1
                    image.x = 0
                    image.y = 0
                }
            }
        }
    }

    Image {
        id: image

        x: 0
        y: 0
        width: parent.width
        height: parent.height

        fillMode: Image.PreserveAspectFit
        smooth: false
        source: "image://raw/image"
    }

    MouseArea {
        property point prevPosition

        anchors.fill: parent
        onWheel: {
            var wheelX = (wheel.x - (image.x + image.width / 2))
            var wheelY = (wheel.y - (image.y + image.width / 2))
            var delta = wheel.angleDelta.y / 1000
            image.scale += delta
            if(image.scale < 1)
                image.scale = 1
            else {
                var newWheelX = wheelX * (image.scale / (image.scale - delta))
                var newWheelY = wheelY * (image.scale / (image.scale - delta))
                image.x += wheelX - newWheelX
                image.y += wheelY - newWheelY
            }
        }

        onPressed: {
            prevPosition = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged: {
            var position = Qt.point(mouse.x, mouse.y)
            var deltaX = (position.x - prevPosition.x)
            var deltaY = (position.y - prevPosition.y)
            prevPosition = position
            image.x += deltaX
            image.y += deltaY
        }
    }

    Shortcut {
        sequence: "F10"
        onActivated: {
            if(appState.state === "fullscreen")
                appState.state = "default"
            else
                appState.state = "fullscreen"
        }
    }

    StateGroup {
        id: appState
        state: "default"
        states: [
            State {
                name: "fullscreen"
                PropertyChanges {
                    target: appWindow
                    visibility: Window.FullScreen
                }
                PropertyChanges {
                    target: appBar
                    visible: false
                }
            }
        ]
    }
}
