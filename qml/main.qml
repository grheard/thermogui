/*
 * Copyright 2021 grheard@gmail.com
 */

import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.VirtualKeyboard 2.1
import QtQuick.VirtualKeyboard.Settings 2.1


Window {
    visible: true
    width: 480
    height: 320
    color: "black"

    Item {
        width: 430
        height: 320
        x: 0
        y: 0

        StackView {
            id: stack
            initialItem: thermoview
            anchors.fill: parent

            pushEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
            pushExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to:0
                    duration: 200
                }
            }
            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
            popExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to:0
                    duration: 200
                }
            }
        }

        StackView {
            id: thermoview
            initialItem: oos

            replaceEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
            replaceExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to:0
                    duration: 200
                }
            }
        }

        Thermostat {
            id: thermostat
            visible: false
        }

        Rectangle {
            id: oos
            visible: true
            x: parent.x
            y: parent.y
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

        Settings {
            id: settings
            visible: false
        }

        InputPanel {
            id: inputPanel
            z: 10
            y: Qt.inputMethod.visible ? parent.height - inputPanel.height : parent.height
            anchors.left: parent.left
            anchors.right: parent.right
            states: State {
                    name: "visible"
                    /*  The visibility of the InputPanel can be bound to the Qt.inputMethod.visible property,
                        but then the handwriting input panel and the keyboard input panel can be visible
                        at the same time. Here the visibility is bound to InputPanel.active property instead,
                        which allows the handwriting panel to control the visibility when necessary.
                    */
                    when: inputPanel.active
                    PropertyChanges {
                        target: inputPanel
                        y: parent.height - inputPanel.height
                    }
            }
            transitions: Transition {
                id: inputPanelTransition
                from: ""
                to: "visible"
                reversible: true
                enabled: !VirtualKeyboardSettings.fullScreenMode
                ParallelAnimation {
                    NumberAnimation {
                        properties: "y"
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }
            }
            Binding {
                target: InputContext
                property: "animating"
                value: inputPanelTransition.running
            }
        }
    }

    Sidebar {
        id: sidebar
        width: 50
        height: 320
        x: 430
        y: 0
    }

    Connections {
        target: mcthermostat

        onSigOutOfService: {
            thermoview.replace(oos)
        }

        onSigInService: {
            thermoview.replace(thermostat)
        }
    }

    Connections {
        target: sidebar

        onSettingsPressed: {
            if (settings.visible) stack.pop()
            else stack.push(settings)
        }
    }
}