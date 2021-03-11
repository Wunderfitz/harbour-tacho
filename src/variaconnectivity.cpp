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
#include "variaconnectivity.h"

#include <QDebug>
#include <QBitArray>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QDBusMessage>
#include <QTime>
#include <QListIterator>

VariaConnectivity::VariaConnectivity(QObject *parent) : QObject(parent)
{
    qDebug() << "Starting up Varia Connectivity";
    this->deviceDiscoveryTimer = new QTimer(this);
    this->deviceDiscoveryTimer->setSingleShot(true);
    connect(deviceDiscoveryTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutDeviceDiscoveryTimer);

    this->deviceConnectionTimer = new QTimer(this);
    connect(deviceConnectionTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutDeviceConnectionTimer);

    QDBusConnection dbusConnection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
    this->mceInterface = new QDBusInterface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", dbusConnection);

    this->screensaverTimer = new QTimer(this);
    this->screensaverTimer->setSingleShot(false);
    this->screensaverTimer->start(15000);
    connect(screensaverTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutScreensaverTimer);
}

VariaConnectivity::~VariaConnectivity()
{
    qDebug() << "Shutting down Varia Connectivity";
    this->enableScreensaver();
}

void VariaConnectivity::initializeRadar()
{
    qDebug() << "Initializing Varia Connectivity...";
    this->adapterInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
    qDebug() << "Bluetooth Device Alias:" << this->adapterInterface->property("Alias");
    QDBusMessage response = this->adapterInterface->call("StartDiscovery");
    qDebug() << "Start Discovery response: " << response.errorMessage();
    this->deviceDiscoveryTimer->start(1000);
}

void VariaConnectivity::disableScreensaver()
{
    qDebug() << "Disabling screensaver";
    this->mceInterface->call("req_display_blanking_pause");
}

void VariaConnectivity::enableScreensaver()
{
    qDebug() << "Enabling screensaver";
    this->mceInterface->call("req_display_cancel_blanking_pause");
}

void VariaConnectivity::onDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    // qDebug() << "Device properties changed" << interface << map << list;
    Q_UNUSED(interface);
    Q_UNUSED(list);

    if (map.contains("Connected")) {
        bool newDeviceConnectionState = map.value("Connected").toBool();
        if (newDeviceConnectionState != this->deviceConnected) {
            qDebug() << "Connection state changed" << newDeviceConnectionState;
            this->deviceConnected = newDeviceConnectionState;
            emit connectionStateChanged(this->deviceConnected);
            if (this->deviceConnected) {
                this->deviceConnectionTimer->stop();
            } else {
                this->deviceConnectionTimer->start(2000);
            }
        }
    }
    if (map.contains(("ServicesResolved"))) {
        bool newServicesResolvedState = map.value("ServicesResolved").toBool();
        if (newServicesResolvedState != this->servicesResolved) {
            qDebug() << "Services resolved state changed" << newServicesResolvedState;
            this->servicesResolved = newServicesResolvedState;
            emit servicesResolvedStateChanged(this->servicesResolved);
            if (this->servicesResolved) {
                this->initializeValueListeners();
            }
        }
    }
}

void VariaConnectivity::onCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    //qDebug() << "Characteristic properties changed" << interface << map << list;
    Q_UNUSED(interface);
    Q_UNUSED(list);

    if (map.contains("Value")) {
        QVariantList currentThreats;
        QByteArray radarData = map["Value"].toByteArray();
        qDebug() << "-----------------------------------------------------------------------------";
        int foundThreats = ( radarData.length() - 1 ) / 3;
        qDebug() << "Threats found:" << foundThreats;
        quint8 idByte = radarData.at(0);
        if (idByte == this->previousIdByte + 2) {
            currentThreats.append(this->previousThreats);
        }
        for (int i = 0; i < foundThreats; i++) {
            quint8 threatNumber = radarData.at(1 + (i * 3));
            quint8 threatDistance = radarData.at(2 + (i * 3));
            quint8 threatSpeed = radarData.at(3 + (i * 3));
            qDebug() << "ID:" << idByte << "Index:" << (i + 1) << ":" << threatNumber << "Threat Distance:" << threatDistance << "Speed:" << threatSpeed;
            QVariantMap currentThreat;
            currentThreat.insert("number", threatNumber);
            currentThreat.insert("distance", threatDistance);
            currentThreat.insert("speed", threatSpeed);
            currentThreats.append(currentThreat);
        }
        qDebug() << "-----------------------------------------------------------------------------";
        this->previousIdByte = idByte;
        this->previousThreats = currentThreats;
        emit threatsDetected(currentThreats);
    }

}

void VariaConnectivity::onBatteryLevelChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    Q_UNUSED(interface);
    Q_UNUSED(list);

    if (map.contains("Percentage")) {
        this->batteryLevel = map.value("Percentage").toInt();
        emit newBatteryLevel(this->batteryLevel);
    }
}

void VariaConnectivity::timeoutDeviceDiscoveryTimer()
{
    qDebug() << "Trying to find our Varia device...";
    this->detectNodeName();
    if (this->variaNodeName.isEmpty()) {
        qDebug() << "Varia device not found, restarting timer...";
        this->deviceDiscoveryTimer->start(1000);
    }
}

void VariaConnectivity::timeoutDeviceConnectionTimer()
{
    if (!this->deviceConnected) {
        this->connectToDevice();
    }
}

void VariaConnectivity::timeoutScreensaverTimer()
{
    this->disableScreensaver();
}

void VariaConnectivity::detectNodeName()
{
    QDBusInterface introspectableInterface("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Introspectable", QDBusConnection::systemBus(), this);
    QDBusReply<QString> introspectResponse = introspectableInterface.call("Introspect");

    QDomDocument introspectDocument;
    introspectDocument.setContent(introspectResponse.value());
    QDomNodeList foundNodes = introspectDocument.elementsByTagName("node");
    for (int i = 0; i < foundNodes.count(); i++) {
        QDomElement foundElement = foundNodes.at(i).toElement();
        QString nodeName = foundElement.attribute("name");

        if (nodeName.startsWith("dev_")) {
            QDBusInterface potentialDeviceInterface("org.bluez", "/org/bluez/hci0/" + nodeName, "org.bluez.Device1", QDBusConnection::systemBus(), this);
            QString deviceName = potentialDeviceInterface.property("Name").toString();
            if (deviceName == "RTL64894") {
                qDebug() << "Varia device found:" << nodeName;
                emit deviceDetected();
                this->variaNodeName = nodeName;
                this->deviceInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + nodeName, "org.bluez.Device1", QDBusConnection::systemBus(), this);
                QStringList argumentMatch;
                argumentMatch << "org.bluez.Device1";
                qDebug() << "Stopping device discovery...";
                QDBusMessage response = this->adapterInterface->call("StopDiscovery");
                qDebug() << "Stop Discovery response: " << response.errorMessage();

                QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + nodeName, "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, QString(), this, SLOT(onDevicePropertiesChanged(QString, QVariantMap, QStringList)));
                this->deviceConnected = this->deviceInterface->property("Connected").toBool();
                this->servicesResolved = this->deviceInterface->property("ServicesResolved").toBool();
                if (this->deviceConnected) {
                    qDebug() << "Wow! Device already connected! :)";
                    emit connectionStateChanged(this->deviceConnected);
                } else {
                    qDebug() << "Not yet connected, trying to connect...";
                    this->deviceConnectionTimer->start(2000);
                }
                if (this->servicesResolved) {
                    this->initializeValueListeners();
                }
                break;
            }
        }
    }
}

void VariaConnectivity::connectToDevice()
{
    qDebug() << "Connecting to device...";
    this->deviceInterface->asyncCall("Connect");
}

void VariaConnectivity::initializeValueListeners()
{
    qDebug() << "Initializing battery listener...";
    this->batteryInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + this->variaNodeName, "org.bluez.Battery1", QDBusConnection::systemBus(), this);
    this->batteryLevel = this->batteryInterface->property("Percentage").toInt();
    qDebug() << "Device battery level:" << this->batteryLevel;
    emit newBatteryLevel(this->batteryLevel);
    QStringList batteryArgumentMatch;
    batteryArgumentMatch << "org.bluez.Battery1";
    QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + this->variaNodeName, "org.freedesktop.DBus.Properties", "PropertiesChanged", batteryArgumentMatch, QString(), this, SLOT(onBatteryLevelChanged(QString, QVariantMap, QStringList)));

    qDebug() << "Initializing threat listener...";
    this->characteristicInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + this->variaNodeName + "/service001d/char001e", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    this->characteristicInterface->call("StartNotify");
    QStringList argumentMatch;
    argumentMatch << "org.bluez.GattCharacteristic1";
    QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + this->variaNodeName + "/service001d/char001e", "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, QString(), this, SLOT(onCharacteristicPropertiesChanged(QString, QVariantMap, QStringList)));
}
