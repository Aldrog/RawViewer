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
        source: "image://raw/image"
    }

    MouseArea {
        property point prevPosition

        anchors.fill: parent
        onWheel: {
            image.scale += wheel.angleDelta.y / 1000
            if(image.scale < 1)
                image.scale = 1
            /* TODO: Correct math for moving towards mouse position
            if(wheel.angleDelta.y > 0) {
                image.x += 1/image.scale * (width  / 2 - wheel.x)
                image.y += 1/image.scale * (height / 2 - wheel.y)
            } else {
                image.x -= 1/image.scale * (width  / 2 - wheel.x)
                image.y -= 1/image.scale * (height / 2 - wheel.y)
            }
            */
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
