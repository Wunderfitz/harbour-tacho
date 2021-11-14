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
import QtQuick 2.6
import Sailfish.Silica 1.0

Row {

    spacing: Theme.paddingMedium

    property var threatData
    property bool dangerous: threatData.speed > blitzerThreshold.value

    Rectangle {
        id: lineLeft
        height: Theme.paddingSmall
        width: ( parent.width - carImage.width - speedLabel.width - ( 3 * Theme.paddingMedium ) ) / 2
        color: dangerous ? Theme.highlightColor : Theme.primaryColor
        radius: height / 3
        anchors.verticalCenter: parent.verticalCenter
    }

    Image {
        id: carImage
        source: dangerous ? "image://theme/icon-m-car?" + Theme.highlightColor : "image://theme/icon-m-car?" + Theme.primaryColor
        width: Theme.itemSizeExtraSmall
        height: width
        anchors.verticalCenter: parent.verticalCenter
    }

    Label {
        id: speedLabel
        text: qsTr("%1 km/h").arg(threatData.speed)
        font.pixelSize: Theme.fontSizeLarge
        font.bold: true
        color: dangerous ? Theme.highlightColor : Theme.primaryColor
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: lineRight
        height: Theme.paddingSmall
        width: ( parent.width - carImage.width - speedLabel.width - ( 3 * Theme.paddingMedium ) ) / 2
        color: dangerous ? Theme.highlightColor : Theme.primaryColor
        radius: height / 3
        anchors.verticalCenter: parent.verticalCenter
    }
}
