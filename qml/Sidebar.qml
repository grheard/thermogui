/*
 * Copyright 2021 grheard@gmail.com
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2


Window {
    id: sidebar
    visible: true
    x: 430
    y: 0
    width: 50
    height: 320
    color: "black" //"#00004d"


    Rectangle {
        anchors {
            fill: parent
            centerIn: parent
        }
        color: "black" //"#00004d"
        // border.color: "white"
        // border.width: 2
        // radius: 5

        Image {
            id: wifi
            visible: true
            width: 44
            height: 44
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
        }

        Image {
            id: settings
            visible: true
            width: 44
            height: 44
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            source: "qrc:/content/system-settings-symbolic.png"
        }
    }

    Connections {
        target: mcwpaif

        onSigSignalPoll: {
            var rssi = parseInt(signalPoll.RSSI);
            if (rssi >= -30) wifi.source = "qrc:/content/wifi-status/network-wireless-signal-excellent-symbolic.png";
            else if (rssi >= -67) wifi.source = "qrc:/content/wifi-status/network-wireless-signal-good-symbolic.png";
            else if (rssi >= -70) wifi.source = "qrc:/content/wifi-status/network-wireless-signal-ok-symbolic.png";
            else if (rssi >= -80) wifi.source = "qrc:/content/wifi-status/network-wireless-signal-weak-symbolic.png";
            else wifi.source = "qrc:/content/wifi-status/network-wireless-signal-none-symbolic.png";
        }
    }
}
