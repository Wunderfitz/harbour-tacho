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

    Component.onCompleted: {
        variaConnectivity.initializeRadar();
    }

    Connections {
        target: variaConnectivity
        onThreatsDetected: {
            threatsListView.model = threats;
            threatRepeater.model = threats;
        }
        onConnectionStateChanged: {
            titlePage.variaConnected = connected;
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

        contentHeight: tachoMainColumn.height

        Column {
            id: tachoMainColumn

            width: titlePage.width
            height: titlePage.height
            spacing: Theme.paddingLarge

            PageHeader {
                id: tachoHeader
                title: "Tacho"
            }

            InfoLabel {
                visible: !titlePage.variaConnected
                text: qsTr("Varia not connected :(")
            }

            Item {
                id: threatsOuterItem
                width: parent.width
                height: parent.height - tachoHeader.height - Theme.paddingLarge
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

            SilicaListView {
                id: threatsListView
                width: parent.width
                height: parent.height - tachoHeader.height - Theme.paddingLarge
                visible: false // titlePage.variaConnected

                clip: true

                delegate: ListItem {
                    id: activeSessionListItem
                    width: parent.width
                    contentHeight: threatsColumn.height + ( 2 * Theme.paddingMedium )

                    Column {
                        id: threatsColumn
                        width: parent.width - ( 2 * Theme.horizontalPageMargin )
                        spacing: Theme.paddingSmall
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter

                        Label {
                            width: parent.width
                            text: "Threat ID: " + modelData.number
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.secondaryColor
                            maximumLineCount: 1
                            elide: Text.ElideRight
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            width: parent.width
                            text: modelData.distance + " m"
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.primaryColor
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            width: parent.width
                            text: modelData.speed + " km/h"
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.primaryColor
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }

                    Separator {
                        id: separator
                        anchors {
                            bottom: parent.bottom
                        }

                        width: parent.width
                        color: Theme.primaryColor
                        horizontalAlignment: Qt.AlignHCenter
                    }

                }

                VerticalScrollDecorator {}
            }
        }
    }
}
