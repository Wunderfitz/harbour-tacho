/*
    Copyright (C) 2021 Sebastian J. Wolf

    This file is part of Tacho.

    Tacho is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tacho is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tacho. If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    property int threatCount: 0
    property bool variaConnected: false

    Connections {
        target: variaConnectivity
        onThreatsDetected: {
            threatCount = threats.length;
        }
        onVariaConnectionStateChanged: {
            variaConnected = connected;
        }
    }

    Image {
        id: imageLoadingBackgroundImage
        asynchronous: true
        fillMode: Image.PreserveAspectFit
        width: parent.height - Theme.paddingLarge
        height: width
        opacity: 0.15
        source: "../../images/background-" + ( Theme.colorScheme ? "black" : "white" ) + ".svg"
        property int sourceDimension: Math.min(parent.width, parent.height) - Theme.paddingMedium
        anchors {
            verticalCenter: parent.verticalCenter
            centerIn: undefined
            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.paddingMedium
        }
        sourceSize {
            width: sourceDimension
            height: sourceDimension
        }
    }


    Label {
        id: label
        anchors.centerIn: parent
        font.pixelSize: Theme.fontSizeLarge
        width: parent.width - ( 2 * Theme.paddingMedium )
        horizontalAlignment: Text.AlignHCenter
        text: variaConnected ? qsTr("Threats: %1").arg(threatCount) : qsTr("Not connected")
        wrapMode: Text.Wrap
    }
}
