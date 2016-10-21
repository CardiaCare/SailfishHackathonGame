import QtQuick 2.0

QtObject {
    property var scene
    property var entities: []

    function generate() {
        var component = Qt.createComponent("Entity.qml")
        entities.push(component.createObject(scene, {"idinsib": 1, "x": 100, "y": 100, "z": 0, "player": 1, "score": 10}));
        entities.push(component.createObject(scene, {"idinsib": 2,"x": 300, "y": 250, "z": 0, "player": 2, "score": 2}));
        entities.push(component.createObject(scene, {"idinsib": 3,"x": 250, "y": 180, "z": 0, "player": 1, "score": 14}));
        entities.push(component.createObject(scene, {"idinsib": 4,"x": 400, "y": 400, "z": 0, "player": 2, "score": 5}));
    }

    function getPointed(position){
        var pointedEntity = null
        entities.forEach(function(entity){
            if (position.x >= entity.x && position.x <= entity.x+80 && position.y >= entity.y && position.y <= entity.y+80) {
                    //entity.contains(mapToItem(entity, position.x, position.y))){
                pointedEntity = entity
            }
        })
        return pointedEntity
    }


    function createParticleObjects(startX, startY, endX, endY, charge, nextplayer) {
        var particle = Qt.createComponent("Particle.qml");

        var sprite2 = particle.createObject(scene, {"x": startX, "y": startY, "endX":endX, "endY":endY, "charge":charge, "nextplayer":nextplayer});

        if (sprite2 == null) {
            // Error Handling
            console.log("Error creating object");
        }

    }



    function changeScore(startPosition){
        var startEntity;
        startEntity = getPointed(startPosition);

        var charge = startEntity.score/2;
        startEntity.score -= charge;
        return charge;

    }

    function addCharge(finX, finY, charge, nextplayer){
        //console.log(nextplayer.toString());
        //console.log(Qt.point(finX, finY));

        var finishEntity = getPointed(Qt.point(finX, finY));

        if (finishEntity && finishEntity.player){
            console.log(finishEntity.player.toString())}

        if (nextplayer == finishEntity.player){
            finishEntity.score += charge;
        }
        else if(finishEntity.score < charge){
            var endScore = charge - finishEntity.score
            finishEntity.score += charge;
            finishEntity.player = nextplayer;

        } else {
            finishEntity.score -= charge;
        }
    }


    function checkWinner(){
        var firstPlayer = entities[0];
        entities.forEach(function(entity){
            if (entity.player !== firstPlayer.player) {
               return false;
            }
        })
        return true;
    }
}
