/*
 * Copyright 2021 grheard@gmail.com
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

Window {
    visible: true
    width: 480
    height: 320
    color: "black"

    Sidebar {
        visible: true
    }

    Thermostat {
        id: thermostat
        visible: false
    }

    Rectangle {
        id: oos
        visible: true
        x: thermostat.x
        y: thermostat.y
        width: thermostat.width
        height: thermostat.height
        color: "black"
        Text {
            anchors {
                fill: parent
                centerIn: parent
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            wrapMode: Text.WordWrap
            font.pixelSize: 60
            text: "Thermostat Out Of Service"
        }
    }

    Connections {
        target: mcthermostat

        onSigOutOfService: oos.visible = true
        onSigInService: oos.visible = false
    }
}