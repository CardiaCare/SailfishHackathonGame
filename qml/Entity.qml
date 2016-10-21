import QtQuick 2.0

Image {
    id:virus

    source: "green.png"

    property int player
    //color: player === 1 ? "red" : "blue"
    width: 50
    height: 50
    property int score:0


    transform: Rotation{
        origin{
            x: virus.width/2
            y: virus.height
        }
        axis{x:1;y:0;z:0}
        angle:5
    }


    Text {
        id: scoreText
        text: qsTr(score.toString())
    }

    Timer {
        interval: 1000
        onTriggered:{
            virus.score += 1
            scoreText.text =  virus.score.toString()
        }
        running: true; repeat: true
    }
}
