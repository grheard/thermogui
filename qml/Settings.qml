/*
 * Copyright 2021 grheard@gmail.com
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3


Item {
    id: settings
    // color: "black"

    RowLayout {
        // anchors {
        //     //top: parent.top
        //     horizontalCenter: parent.horizontalCenter
        // }

        Image {
            width: 44
            height: 44
            source: "qrc:/content/system-settings-symbolic.png"
        }

        Text {
            color: "white"
            font.pixelSize: 48
            text: "Settings"
        }

        TextField {
            width: parent.width
            placeholderText: "One line field"
        }
    }
}
