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

Page {
    id: titlePage

    allowedOrientations: Orientation.All

    Component.onCompleted: {
        variaConnectivity.initializeRadar();
    }

    Connections {
        target: variaConnectivity
        onThreatsDetected: {
            threatsListView.model = threats;
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

            SilicaListView {
                id: threatsListView
                width: parent.width
                height: parent.height - tachoHeader.height - Theme.paddingLarge

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
                            text: "Threat ID: " + modelData.threatId
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
