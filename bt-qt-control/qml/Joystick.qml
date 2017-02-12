import QtQuick 2.0

Item {
    id: top
 
    property QtObject socket
    property int headerHeight: 30
    property string socketData: "0:0\n"
    
    property int refreshInterval: 100
    
 
    Item {
        id: header
        
        height: headerHeight
        width: top.width
        anchors.top: top.top
        anchors.left: top.left
        
        Rectangle {
            anchors.fill: parent
            color: "black"    
        }
        
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            
            text: qsTr("RiBot Control")
            color: "white"
            font.pointSize: 17
            font.bold: true
        }
    }
    
    Item {
        id: joystickItem
        
        anchors.top: header.bottom
        anchors.bottom: backButton.top
        anchors.left: top.left
        anchors.right: top.right
        
        width: top.width
        
        Rectangle {
            id: joystick
            
            anchors.centerIn: joystickItem
            height: Math.min(parent.height, parent.width) * 0.9 
            width: height
            
            radius: width / 2
            
            border.width: 2
            border.color: "#212121"
        }
        
        Timer {
            id: sendTimer
            interval: refreshInterval
            running: false
            
            repeat: true
            
            onTriggered: {
                console.log("Sending:", socketData)
                socket.stringData = socketData
            }
        }
        
        ParallelAnimation {
            id: returnAnimation
            NumberAnimation { target: handle; property: "x";
                to: joystick.x + joystick.width * 0.5 - handle.width * 0.5; duration: 200; easing.type: Easing.OutSine }
            NumberAnimation { target: handle; property: "y";
                to: joystick.y + joystick.height * 0.5 - handle.height * 0.5; duration: 200; easing.type: Easing.OutSine }
            
            onStopped: {
                socketData = "0:0:0\n"
                socket.stringData = socketData 
                sendTimer.running = false
            }
        }
        
        Rectangle {
            id: handle
            
            x: joystick.x + joystick.width / 2 - width/2
            y: joystick.y + joystick.height/ 2 - height/2
            
            height: joystick.width * 0.2
            width: height
            
            color: "#1c56f3"
            radius: width / 2
            
            MouseArea {
                property int mcx : top.mapToItem(joystick, handle.x, handle.y).x + handle.width * 0.5 - joystick.width * 0.5
                property int mcy : joystick.height * 0.5 - handle.height - top.mapToItem(joystick, handle.x, handle.y).y
                
                anchors.fill: handle
                drag.target: handle
                drag.maximumX: joystick.x + joystick.width - handle.width
                drag.minimumX: joystick.x
                drag.maximumY: joystick.y + joystick.height - handle.height
                drag.minimumY: joystick.y
                
                onPressed: {
                    returnAnimation.stop()
                    
                    if (socket.connected) {
                        sendTimer.running = true
                    }
                }
                
                onReleased: {
                    returnAnimation.restart()
                    
                    if (socket.connected) {
                        socketData = "0:0:0\n"
                        socket.stringData = socketData 
//                        sendTimer.running = false
                    }
                }
                
                onPositionChanged: {
                    var angSpeedRatio = (angularSpeedFactor * mcx / ((joystick.width - handle.width)* 0.5)) * 255
                    var dirSpeedRatio = (mcy / ((joystick.height - handle.height)* 0.5)) * 255
                    var crc = angSpeedRatio + dirSpeedRatio
                    
                    socketData = qsTr("%1:%2:%3\n").arg(angSpeedRatio).arg(dirSpeedRatio).arg(crc)
                }
            }
        }
    }
    
    Button {
        id: backButton
        
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 5
        width: 200
        text: "Back"
        color: "white"
        onClicked: {
            swipeView.currentIndex = 0
            socket.connected = false
        }
    }
}
