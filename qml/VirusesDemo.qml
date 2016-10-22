import QtQuick 2.0

Item {
    id: item
    width: 640
    height: 480

    EntityManager {
        id: entityManager
        scene: item
    Component.onCompleted: {
        var maxW = width
        var maxH = height
        var posX, posY, player, score
        var lastPosX = 0
        var lastPosY = 0
        var i
        var playerCount = []
        for(i = 0; i < 4; i++) {
            posX = Math.random() * (maxW - 100) + 50
            posY = Math.random() * (maxH - 100) + 50
            score = Math.random() * 30 + 10
            player = Math.random() * 2 + 1
            if(Math.sqrt(Math.pow((posX - lastPosX),2) + Math.pow((posY - lastPosY),2)) > 100) {
                lastPosX = posX
                lastPosY = posY
                generate(posX,posY,player,score)
            }
            else {
                i = i - 1
            }
        }
    }
    }


    Controller {
        anchors.fill: parent
        z: -1
        controllerPosition: 1
        startRadius: 40
        endRadius: 50
        onHooked: {
            var entity = entityManager.getPointed(position)
            if (entity !== null) {
                var entityPosition = Qt.point(entity.x + entity.width / 2, entity.y + entity.height / 2)
                var entityPlayer = entity.player
                if (entityPlayer === 1) {
                    startPosition = entityPosition
                    endPosition = startPosition
                    status = 1

                }
            }
        }
        onMoved: {
            var entity = entityManager.getPointed(position)
            if (entity !== null) {
                var entityPosition = Qt.point(entity.x + entity.width / 2, entity.y + entity.height / 2)
                var entityPlayer = entity.player
                if (entityPlayer !== 1) {
                    endPosition = entityPosition
                    status = 2
                }
            }
            else {
                endPosition = position
                status = 1
            }

        }

    }

//    Text {
//        id: totalTime
//        text: qsTr(totalTime.toString())
//    }

    Timer {
        id: totalTimer
        interval: 30000
        onTriggered:{

            entityManager.checkWinner()
            //totalTime.text =  .toString()
        }
        running: true; repeat: false
    }


}
