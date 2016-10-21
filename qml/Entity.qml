import QtQuick 2.0

AnimatedImage {
    id:virus
    scale:1
    property int player
    //color: player === 1 ? "red" : "blue"
    width: 80
    height: 80

   source: {
        if (player == 1)
            "green.gif"
        if (player == 2)
             "red.gif"
    }


    property int score:0
    property int idinsib

//    onScoreChanged: {
//        if (score >= 50)
//            scale = 1.4
//        if (score <= 10)
//            scale = 0.6
//        if(score >10 && score < 50)
//            scale = 1
//    }

    NumberAnimation { target: virus; property: "scale"; to: scale; duration: 1000 }
//    states: State {
//        name: "rotated"
//        transform: Scale { xScale: 3}
//    }

    Text {
        id: scoreText
        color: "white"
        text: qsTr(score.toString())
        anchors {  centerIn: parent }
        font.bold: true
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
