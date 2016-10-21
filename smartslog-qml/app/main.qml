import QtQuick 2.5
import QtQuick.Window 2.2
import SmartSlog 1.0

Window {
    visible: true

    MainForm {
        anchors.fill: parent
        mouseArea.onClicked: {
            Qt.quit();
        }
    }
    SmartSpace {
        address: "78.46.130.194"
        name: "X"
        port: 10010
    }
}

