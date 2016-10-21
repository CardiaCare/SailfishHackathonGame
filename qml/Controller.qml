import QtQuick 2.0

Canvas {
    property int status: 0

    property point startPosition
    property point endPosition
    property int startRadius
    property int endRadius

    property int controllerPosition: 0

    signal hooked(point position)
    signal moved(point position)
    signal pointed(point position)

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        if (status > 0) {
            ctx.strokeStyle = "white"
            context.beginPath()
            context.arc(startPosition.x, startPosition.y, startRadius, 0, 3 * Math.PI, false);
            context.lineWidth = 3;
            context.stroke()
            ctx.strokeStyle = "white"
            context.beginPath()
            context.moveTo(startPosition.x, startPosition.y);
            context.lineTo(endPosition.x, endPosition.y);
            context.lineWidth = 3;
            context.stroke()
        }
        if (status > 1) {
            ctx.strokeStyle = "white"
            context.beginPath()
            context.arc(endPosition.x, endPosition.y, endRadius, 0, 3 * Math.PI, false);
            context.lineWidth = 3;
            context.stroke()
        }
    }

    MouseArea {
        anchors.fill: parent
        z: controllerPosition
        onPressed: {
            console.log("onPressed")
            var startPosition = Qt.point(mouseX, mouseY)
            hooked(startPosition)
        }
        onReleased: {
            console.log("onReleased")
            var endPosition = Qt.point(mouseX, mouseY)
            if (status > 0) {
                pointed(endPosition)
                requestPaint()

                var startEntity = entityManager.getPointed(startPosition)
                var endEntity = entityManager.getPointed(endPosition)
                console.log(startEntity.idinsib+ " " +endEntity.idinsib)
                if (startEntity.idinsib !== endEntity.idinsib){
                    var charge = entityManager.changeScore(startPosition,endPosition)
                    entityManager.createParticleObjects(startEntity.x+80, startEntity.y+80, endEntity.x, endEntity.y,charge, startEntity.player)
                }

            }
            status = 0

        }
        onPositionChanged: {
            if (status > 0) {
                moved(Qt.point(mouseX, mouseY))
                requestPaint()
            }
        }
    }

    onStatusChanged: {
        requestPaint()
    }
}

