import QtQuick 2.0
import QuickPromise 1.0
import "virusgame.js" as Logic

Image {
    id: particleImg

    width: 10
    height: 10

    property int startX
    property int startY
    property int endX
    property int endY
    property int nextplayer
    property int charge

    source: "Particle.png"

    SequentialAnimation{
        id: parAn
        running: true
        ParallelAnimation{

            PropertyAnimation {
                target: particleImg
                property: "x"
                from: x
                to: endX
                duration: 3000
                //running: true
            }

            PropertyAnimation{
                target: particleImg
                property: "y"
                from: y
                to: endY
                duration: 3000
                //running: true
            }
        }
        PropertyAnimation {
            target: particleImg
            property: "opacity"
            to:0
            duration: 50
        }

        onRunningChanged:{
            if (!parAn.running){

                entityManager.addCharge(endX, endY, charge, nextplayer);


                if (checkWinner()){
                    console.log("winner");
                }
            }
        }
    }


    onOpacityChanged: {
        if (opacity == 0.0) {
            particleImg.destroy()
        }
    }

}

//        NumberAnimation on x{
//            from: startX
//            to: endX
//            duration: 200
//            easing.type: Easing.InOutQuad
//        }
//        NumberAnimation on y {
//            from: startY
//            to: endY
//            duration: 200
//            easing.type: Easing.InOutQuad
//        }



//    Timer {
//        interval: Logic.getRandomRound(350, 1500); running: true; repeat: false
//        onTriggered: root.opacity = 0.0
//    }

//    // Когда изображение станет прозрачным. мы уничтожим сам объект
//    onOpacityChanged: {
//        if (opacity == 0.0) {
//            Logic.gameState.scores--
//            Logic.destroyParticle(row, col)
//            root.destroy()
//        }
//    }

//    MouseArea {
//        id: mouseArea
//        anchors.fill: parent
//        // Данной свойство определяет, будут ли передаваться события клика
//        // в ниже лежащие элементы, то есть те, которые по координате z
//        // находятся ниже мишени
//        propagateComposedEvents: true
//        onClicked: {
//            var scores =  isHit(root.width/2, root.height/2, mouse.x, mouse.y);
//            if (scores != 0) {
//                // Если количество очков отлично от нуля, то фиксируем попадание
//                // Увеличиваем счёт и уничтожаем мишень
//                Logic.gameState.scores += scores;
//                Logic.destroyParticle(row, col);
//                root.destroy();
//            } else {
//                // В противном случае передаём событие клика в ниже лежащие элементы
//                // по координате z
//                mouse.accepted = false;
//            }
//        }
//    }

    // Задаём поведение анимации свойства прозрачности объекта
//    Behavior on opacity { PropertyAnimation { duration: 250 } }

