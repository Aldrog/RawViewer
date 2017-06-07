import QtQuick 2.6
import QtQuick.Window 2.2

Window {
    visible: true
    width: 1224
    height: 1024
    title: qsTr("Hello LPS")

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
}
