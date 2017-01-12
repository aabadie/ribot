import QtQuick 2.7
import QtBluetooth 5.3

Item {
    id: top
    
    property QtObject socket
    property int headerHeight: 30

    BluetoothDiscoveryModel {
        id: btModel
        uuidFilter: "00001101-0000-1000-8000-00805F9B34FB" // Only SPP Profile
        running: true
//        discoveryMode: BluetoothDiscoveryModel.DeviceDiscovery
        discoveryMode: BluetoothDiscoveryModel.MinimalServiceDiscovery
//        discoveryMode: BluetoothDiscoveryModel.FullServiceDiscovery
        onDiscoveryModeChanged: console.log("Discovery mode: " + discoveryMode)
        onServiceDiscovered: console.log("Found new service " + service.deviceAddress + " " + service.deviceName + " " + service.serviceName);
        onDeviceDiscovered: console.log("New device: " + device)
        onErrorChanged: {
            switch (btModel.error) {
            case BluetoothDiscoveryModel.PoweredOffError:
                console.log("Error: Bluetooth device not turned on"); break;
            case BluetoothDiscoveryModel.InputOutputError:
                console.log("Error: Bluetooth I/O Error"); break;
            case BluetoothDiscoveryModel.InvalidBluetoothAdapterError:
                console.log("Error: Invalid Bluetooth Adapter Error"); break;
            case BluetoothDiscoveryModel.NoError:
                break;
            default:
                console.log("Error: Unknown Error"); break;
            }
        }
    }
    
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
            
            text: qsTr("RiBot selection")
            color: "white"
            font.pointSize: 17
            font.bold: true
        }
    }

    ListView {
        id: mainList
        width: top.width
        anchors.top: header.bottom
        anchors.bottom: devButton.top
        anchors.bottomMargin: 10
        clip: true
        spacing: 2

        model: btModel
        delegate: Rectangle {
            id: btDelegate
            
            width: mainList.width
            height: column.height + 10
            
            clip: true
            border.width: 2
            border.color: "#212121"
            
            color: mouseArea.pressed ? "#888888"
                                     : "#eeeeee"
            
            Image {
                id: bticon
//                source: "qrc:/default.png";
                width: bttext.height;
                height: bttext.height;
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 5
            }

            Column {
                id: column
                anchors.left: bticon.right
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                
                Text {
                    id: bttext
                    text: qsTr("Device: %1").arg(deviceName ? deviceName : name)
                    font.pointSize: 16
                }

                Text {
                    id: btaddress
                    text: qsTr("MAC: %1").arg(service.deviceAddress)
                    font.pointSize: 14
                }
            }
            Behavior on height { NumberAnimation { duration: 200} }

            MouseArea {
                id: mouseArea
                
                anchors.fill: parent
                onClicked: {
                    console.log("Connecting to service")
                    socket.setService(service)
                    socket.connected = true
                }
            }
        }
        focus: true
    }

    Button {
        id: devButton
        
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200
        text: btModel.running ? "Scanning" : "Scan"
        color: !btModel.running ? "white" : "#515151"
        onClicked: {
            btModel.running = true
            btModel.discoveryMode = BluetoothDiscoveryModel.MinimalServiceDiscovery
        }
        
        SequentialAnimation on color {
            id: busyThrobber
            ColorAnimation { easing.type: Easing.InOutSine; from: "#515151"; to: "white"; duration: 1000 }
            ColorAnimation { easing.type: Easing.InOutSine; to: "#515151"; from: "white"; duration: 1000 }
            loops: Animation.Infinite
            running: btModel.running
        }
    }
}
