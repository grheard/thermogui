/*
 * Copyright 2021 grheard@gmail.com
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3


Rectangle {
    id: thermostat
    color: "black"

    Rectangle {
        x: 0
        y: 0
        width: parent.width - x
        height: parent.height - 125 - y
        color: "black"
        radius: 5

        RowLayout {
            spacing: 40
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 1

                Text {
                    id: temperatureText
                    font.pixelSize: 120
                    Behavior on x { NumberAnimation{ easing.type: Easing.OutCubic} }
                    Layout.alignment: Qt.AlignCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
                    text: "00.0"
                }
                Text {
                    id: humidityText
                    font.pixelSize: 25
                    Behavior on x { NumberAnimation{ easing.type: Easing.OutCubic} }
                    Layout.alignment: Qt.AlignCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
                    text: "00.0%"
                }
            }

            ColumnLayout {
                spacing: 5

                Rectangle {
                    id: rectCool
                    color: "black"
                    radius: 5
                    Layout.minimumWidth: 100
                    Layout.minimumHeight: 80
                    Layout.alignment: Qt.AlignHCenter

                    ColumnLayout {
                        spacing: 0
                        anchors.centerIn: parent

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            bottomPadding: 0
                            font.pixelSize: 24
                            color: "white"
                            text: "cool"
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            topPadding: 0
                            id: setpointCoolText
                            font.pixelSize: 40
                            horizontalAlignment: Text.AlignHCenter
                            color: "white"
                            text: "00.0"
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            settingsPopupText.text = "cool";
                            settingsPopupSpinBox.value = setpointCoolText.text;
                            settingsPopup.open();
                        }
                    }
                    SequentialAnimation {
                        id: animCool
                        loops: Animation.Infinite
                        PropertyAnimation {
                            target: rectCool
                            property: "color"
                            to: "blue"
                            duration: 1000

                        }
                        PropertyAnimation {
                            target: rectCool
                            property: "color"
                            to: "black"
                            duration: 1000

                        }
                    }
                }

                Rectangle {
                    id: rectHeat
                    color: "black"
                    radius: 5
                    Layout.minimumWidth: 100
                    Layout.minimumHeight: 80
                    Layout.alignment: Qt.AlignHCenter

                    ColumnLayout {
                        spacing: 0
                        anchors.centerIn: parent

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            bottomPadding: 0
                            font.pixelSize: 24
                            color: "white"
                            text: "heat"
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            topPadding: 0
                            id: setpointHeatText
                            font.pixelSize: 40
                            horizontalAlignment: Text.AlignHCenter
                            color: "white"
                            text: "00.0"
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            settingsPopupText.text = "heat";
                            settingsPopupSpinBox.value = setpointHeatText.text;
                            settingsPopup.open();
                        }
                    }
                    SequentialAnimation {
                        id: animHeat
                        loops: Animation.Infinite
                        PropertyAnimation {
                            target: rectHeat
                            property: "color"
                            to: "red"
                            duration: 1000

                        }
                        PropertyAnimation {
                            target: rectHeat
                            property: "color"
                            to: "black"
                            duration: 1000

                        }
                    }
                }
            }
        }
    }

    Rectangle {
        x: 0
        y: parent.height - 125
        width: parent.width - x
        height: 125
        color: "black"
        radius: 5

        RowLayout {
            spacing: 20
            anchors.centerIn: parent

            ColumnLayout {
                Text {
                    Layout.alignment: Qt.AlignLeft
                    bottomPadding: 1
                    font.pixelSize: 20
                    color: "white"
                    text: "mode"
                }
                RoundButton {
                    topPadding: 1
                    id: settingModeText
                    font.pixelSize: 32
                    Layout.alignment: Qt.AlignCenter
                    Layout.minimumWidth: 100
                    Layout.margins: 0
                    text: ""
                    palette.base: "black"
                    palette.text: "white"
                    palette.button: "#000099"
                    palette.buttonText: "white"
                    radius: 5
                    onClicked: mcthermostat.slotModeChange(text)
                }
            }

            ColumnLayout {
                Text {
                    Layout.alignment: Qt.AlignLeft
                    bottomPadding: 1
                    font.pixelSize: 20
                    color: "white"
                    text: "fan"
                }
                RoundButton {
                    topPadding: 1
                    id: settingFanText
                    font.pixelSize: 32
                    Layout.alignment: Qt.AlignCenter
                    Layout.minimumWidth: 100
                    Layout.margins: 0
                    text: ""
                    palette.base: "black"
                    palette.text: "white"
                    palette.button: "#000099"
                    palette.buttonText: "white"
                    radius: 5
                    onClicked: mcthermostat.slotFanChange(text)
                }
            }
        }
    }

    Popup {
        id: settingsPopup
        modal: true
        closePolicy: Popup.NoAutoClose
        width: 380
        height: 220
        x: 50
        y: 50

        background: Rectangle {
            color: "black"
            border.color: "white"
            border.width: 2
            radius: 5
        }

        ColumnLayout {
            spacing: 10
            anchors.fill: parent

            Text {
                id: settingsPopupText
                font.pixelSize: 24
                color: "white"
            }

            SpinBox {
                Layout.alignment: Qt.AlignCenter
                id: settingsPopupSpinBox
                font.pixelSize: 90
                editable: false
                palette.base: "black"
                palette.text: "white"
                palette.button: "#3366ff"
                palette.buttonText: "white"

                onValueModified: settingsPopupTimer.restart()
            }

            Timer {
                id: settingsPopupTimer
                interval: 3000
                onTriggered: {
                    settingsPopup.close();
                }
            }
        }

        onOpened: settingsPopupTimer.start()

        onAboutToHide: mcthermostat.slotSettingsChange(settingsPopupText.text,settingsPopupSpinBox.value)
    }

    Connections {
        property var thermostatState: ""
        property var thermostatOutput: ""

        function handleMode(value) {
            if (value === 'auto' || value === 'cool') rectCool.opacity = 1
            else rectCool.opacity = 0.5
            if (value === 'auto' || value === 'heat') rectHeat.opacity = 1
            else rectHeat.opacity = 0.5
            settingModeText.text = value
        }
        function handleFan(value) {
            settingFanText.text = value;
        }
        function handleStateColor() {
            if (thermostatState !== 'idle' && thermostatOutput === 'locked') {
                if (thermostatState === 'heat') animHeat.running = true
                if (thermostatState === 'cool') animCool.running = true
            }
            else {
                animHeat.running = false
                animCool.running = false
            }

            if (animCool.running === false) {
                if (thermostatState === 'cool') rectCool.color = 'blue'
                else rectCool.color = 'black'
            }

            if (animHeat.running === false) {
                if (thermostatState === 'heat') rectHeat.color = 'red'
                else rectHeat.color = 'black'
            }
        }

        target: mcthermostat
        onSigTemperature: temperatureText.text = value
        onSigHumidity: humidityText.text = value + "%"

        onSigState: {
            thermostatState = value
            handleStateColor()
        }
        onSigOutput: {
            thermostatOutput = value
            handleStateColor()
        }
        onSigSettingsHeat: setpointHeatText.text = value
        onSigSettingsCool: setpointCoolText.text = value
        onSigSettingsMode: handleMode(value)
        onSigSettingsFan: handleFan(value)
        onSigFan: handleFan(value)
    }
}
