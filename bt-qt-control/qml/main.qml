import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtBluetooth 5.2


ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("RiBot")
    
    Rectangle {
        anchors.fill: parent
        
        color: "#212121"
    }
    
    BluetoothSocket {
        id: socket
        connected: true
    
        onSocketStateChanged: {
            switch (socketState) {
            case BluetoothSocket.NoServiceSet:
                console.log("No service set")
                break;
            case BluetoothSocket.Unconnected:
                console.log("Unconnected")
                break;
            case BluetoothSocket.ServiceLookup:
                console.log("Service lookup")
                break;
            case BluetoothSocket.Connecting:
                console.log("Connecting")
                break;
            case BluetoothSocket.Connected:
                console.log("Connected")
                break;
            case BluetoothSocket.Closing:
                console.log("Closing")
                break;
            case BluetoothSocket.Listening:
                console.log("Listening")
                break;
            case BluetoothSocket.Bound:
                console.log("Bound")
                break;
            default:
                console.log("Unknown state")
            }

            if (connected) {
                console.log("Connected to service")
                swipeView.currentIndex = 1
            }
            else {
                console.log("Disconnected from service")
                swipeView.currentIndex = 0
            }
        }
    }
    
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: 0
        
        Scanner {
            socket: socket
            headerHeight: 35
        }
        
        Joystick {
            socket: socket
            headerHeight: 35
        }
    }
    
    
//    PageIndicator {
//        id: indicator
    
//        count: swipeView.count
//        currentIndex: swipeView.currentIndex
    
//        anchors.bottom: swipeView.bottom
//        anchors.horizontalCenter: parent.horizontalCenter
//    }
}
