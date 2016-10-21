import QtQuick 2.0

Rectangle {
    id:virus

    property int player
    color: player === 1 ? "red" : "blue"
    width: 50
    height: 50
    property int score:0
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
