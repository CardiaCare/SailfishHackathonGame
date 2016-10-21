import QtQuick 2.0

Rectangle {
    id: item
    width: 640
    height: 480
    color: "black"

    EntityManager {
        id: entityManager
        scene: item
        Component.onCompleted: {
            generate()
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

            checkWinner()
            //totalTime.text =  .toString()
        }
        running: true; repeat: false
    }


}
