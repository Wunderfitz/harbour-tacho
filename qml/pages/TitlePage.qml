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
import "../components"

Page {
    id: titlePage

    allowedOrientations: Orientation.All

    property bool variaConnected;
    property int variaBatteryLevel;

    Component.onCompleted: {
        variaConnectivity.initializeDevices();
    }

    Connections {
        target: variaConnectivity
        onThreatsDetected: {
            threatRepeater.model = threats;
        }
        onVariaConnectionStateChanged: {
            titlePage.variaConnected = connected;
        }
        onVariaNewBatteryLevel: {
            titlePage.variaBatteryLevel = batteryLevel;
        }
    }

    Connections {
        target: speedometer
        onNewSpeed: {
            speedLabel.text = Math.round(speed);
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("About Tacho")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        contentHeight: variaColumn.height

        Grid {
            id: mainGrid
            width: titlePage.width
            height: titlePage.height
            rows: titlePage.isPortrait ? 2 : 1
            columns: titlePage.isPortrait ? 1 : 2

            readonly property real columnWidth: width / columns
            readonly property real rowHeight: height / rows

            Column {
                id: speedColumn
                width: titlePage.isPortrait ? mainGrid.columnWidth : (speedLabel.height + kmhLabel.height)
                height: titlePage.isPortrait ? implicitHeight : mainGrid.rowHeight

                Label {
                    id: speedLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "0"
                    width: parent.columnWidth
                    font.pixelSize: Theme.fontSizeHuge * 5
                    font.bold: true
                    color: Theme.primaryColor
                }

                Label {
                    id: kmhLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("km/h")
                    width: parent.columnWidth
                    font.pixelSize: Theme.fontSizeLarge
                    font.bold: true
                    color: Theme.primaryColor
                }

                Item {
                    id: statusRow
                    width: parent.width - ( 2 * Theme.horizontalPageMargin )
                    height: titlePage.isPortrait ? Theme.itemSizeSmall : ( speedColumn.height - speedLabel.height - kmhLabel.height )
                    anchors.horizontalCenter: parent.horizontalCenter

                    Label {
                        text: qsTr("Varia Battery Level: %1\%").arg(titlePage.variaBatteryLevel)
                        width: parent.width
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.primaryColor
                        visible: titlePage.variaConnected
                        horizontalAlignment: Text.AlignHCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingMedium
                    }
                }
            }

            Column {
                id: variaColumn

                width: titlePage.isPortrait ? mainGrid.columnWidth : (parent.width - speedColumn.width)
                height: titlePage.isPortrait ? (parent.height - speedColumn.height) : mainGrid.rowHeight
                spacing: Theme.paddingSmall

                InfoLabel {
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    visible: !titlePage.variaConnected
                    text: qsTr("Varia not connected :(")
                }

                Item {
                    id: threatsOuterItem
                    width: parent.width
                    height: parent.height
                    visible: titlePage.variaConnected

                    Item {
                        id: threatsItem
                        width: parent.width - ( 2 * Theme.horizontalPageMargin )
                        height: parent.height - ( 2 * Theme.paddingLarge )
                        anchors.centerIn: parent

                        Rectangle {
                            id: distanceIndicatorRectangle
                            width: Theme.horizontalPageMargin
                            height: parent.height
                            color: Theme.primaryColor
                            radius: Theme.horizontalPageMargin / 3
                        }

                        Image {
                            id: personImage
                            source: "image://theme/icon-m-contact"
                            width: Theme.itemSizeExtraSmall
                            height: width
                            anchors.top: distanceIndicatorRectangle.top
                            anchors.left: distanceIndicatorRectangle.right
                            anchors.leftMargin: Theme.paddingMedium
                        }

                        Label {
                            text: qsTr("50 m")
                            anchors.left: distanceIndicatorRectangle.right
                            anchors.leftMargin: Theme.paddingMedium
                            y: ( distanceIndicatorRectangle.height / 3 ) - height
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.primaryColor
                        }

                        Label {
                            text: qsTr("100 m")
                            anchors.left: distanceIndicatorRectangle.right
                            anchors.leftMargin: Theme.paddingMedium
                            y: ( distanceIndicatorRectangle.height / 3 * 2 ) - height
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.primaryColor
                        }

                        Label {
                            id: maxMeterLabel
                            text: qsTr("150 m")
                            anchors.left: distanceIndicatorRectangle.right
                            anchors.leftMargin: Theme.paddingMedium
                            anchors.bottom: distanceIndicatorRectangle.bottom
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.primaryColor
                        }

                        Repeater {
                            id: threatRepeater
                            anchors.left: maxMeterLabel.right
                            anchors.leftMargin: Theme.paddingMedium
                            anchors.right: parent.right
                            height: parent.height
                            delegate: Threat {
                                width: threatRepeater.width
                                threatData: modelData
                                anchors.left: maxMeterLabel.right
                                anchors.leftMargin: Theme.paddingMedium
                                y: threatRepeater.height / 150 * modelData.distance
                            }

                        }
                    }
                }

            }

        }


    }
}
