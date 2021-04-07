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
    qDebug() << "Starting up Device Connectivity";
    this->variaDeviceDiscoveryTimer = new QTimer(this);
    this->variaDeviceDiscoveryTimer->setSingleShot(true);
    connect(variaDeviceDiscoveryTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutVariaDeviceDiscoveryTimer);

    this->variaDeviceConnectionTimer = new QTimer(this);
    connect(variaDeviceConnectionTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutVariaDeviceConnectionTimer);

    qDebug() << "Starting up Cycliq Connectivity";
    this->cycliqDeviceDiscoveryTimer = new QTimer(this);
    this->cycliqDeviceDiscoveryTimer->setSingleShot(true);
    connect(cycliqDeviceDiscoveryTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutCycliqDeviceDiscoveryTimer);

    this->cycliqDeviceConnectionTimer = new QTimer(this);
    connect(cycliqDeviceConnectionTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutCycliqDeviceConnectionTimer);

    QDBusConnection dbusConnection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
    this->mceInterface = new QDBusInterface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", dbusConnection);

    this->screensaverTimer = new QTimer(this);
    this->screensaverTimer->setSingleShot(false);
    this->screensaverTimer->start(15000);
    connect(screensaverTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutScreensaverTimer);

    this->alertsCleanupTimer = new QTimer(this);
    this->alertsCleanupTimer->setSingleShot(false);
    this->alertsCleanupTimer->start(600000);
    connect(alertsCleanupTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutAlertsCleanupTimer);

    this->mediaPlayer = new QMediaPlayer();
    this->mediaPlayer->setMedia(QUrl::fromLocalFile("/usr/share/sounds/jolla-ringtones/stereo/poppy-red-tone-1.ogg"));
    this->mediaPlayer->setVolume(50);
}

VariaConnectivity::~VariaConnectivity()
{
    qDebug() << "Shutting down Device Connectivity";
    this->enableScreensaver();
}

void VariaConnectivity::initializeDevices()
{
    qDebug() << "Initializing Varia Connectivity...";
    this->adapterInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
    qDebug() << "Bluetooth Device Alias:" << this->adapterInterface->property("Alias");
    QDBusMessage response = this->adapterInterface->call("StartDiscovery");
    qDebug() << "Start Discovery response: " << response.errorMessage();
    this->variaDeviceDiscoveryTimer->start(1000);
    this->cycliqDeviceDiscoveryTimer->start(1000);
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

void VariaConnectivity::onVariaDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    // qDebug() << "Device properties changed" << interface << map << list;
    Q_UNUSED(interface);
    Q_UNUSED(list);

    if (map.contains("Connected")) {
        bool newDeviceConnectionState = map.value("Connected").toBool();
        if (newDeviceConnectionState != this->variaDeviceConnected) {
            qDebug() << "Connection state changed" << newDeviceConnectionState;
            this->variaDeviceConnected = newDeviceConnectionState;
            emit variaConnectionStateChanged(this->variaDeviceConnected);
            if (this->variaDeviceConnected) {
                this->variaDeviceConnectionTimer->stop();
            } else {
                this->variaDeviceConnectionTimer->start(2000);
            }
        }
    }
    if (map.contains(("ServicesResolved"))) {
        bool newServicesResolvedState = map.value("ServicesResolved").toBool();
        if (newServicesResolvedState != this->variaServicesResolved) {
            qDebug() << "Services resolved state changed" << newServicesResolvedState;
            this->variaServicesResolved = newServicesResolvedState;
            emit variaServicesResolvedStateChanged(this->variaServicesResolved);
            if (this->variaServicesResolved) {
                this->initializeVariaValueListeners();
            }
        }
    }
}

void VariaConnectivity::onVariaCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    //qDebug() << "Characteristic properties changed" << interface << map << list;
    Q_UNUSED(interface);
    Q_UNUSED(list);

    if (map.contains("Value")) {
        QVariantList currentThreats;
        QByteArray radarData = map["Value"].toByteArray();
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
            if (threatSpeed > 70 && !this->sentAlerts.contains(threatNumber)) {
                qDebug() << "Uh, fast! Sending alert...";
                this->sentAlerts.append(threatNumber);
                this->mediaPlayer->play();
            }
        }
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
        emit variaNewBatteryLevel(this->batteryLevel);
    }
}

void VariaConnectivity::onCycliqDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    Q_UNUSED(interface);
    Q_UNUSED(list);

    if (map.contains("Connected")) {
        bool newDeviceConnectionState = map.value("Connected").toBool();
        if (newDeviceConnectionState != this->cycliqDeviceConnected) {
            qDebug() << "Cycliq connection state changed" << newDeviceConnectionState;
            this->cycliqDeviceConnected = newDeviceConnectionState;
            emit cycliqConnectionStateChanged(this->cycliqDeviceConnected);
            if (this->cycliqDeviceConnected) {
                this->cycliqDeviceConnectionTimer->stop();
            } else {
                this->cycliqDeviceConnectionTimer->start(2000);
            }
        }
    }
    if (map.contains(("ServicesResolved"))) {
        bool newServicesResolvedState = map.value("ServicesResolved").toBool();
        if (newServicesResolvedState != this->cycliqServicesResolved) {
            qDebug() << "Cycliq services resolved state changed" << newServicesResolvedState;
            this->cycliqServicesResolved = newServicesResolvedState;
            emit cycliqServicesResolvedStateChanged(this->cycliqServicesResolved);
            if (this->cycliqServicesResolved) {
                this->initializeCycliqValueListeners();
            }
        }
    }
}

void VariaConnectivity::onCycliqCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{

}

void VariaConnectivity::timeoutVariaDeviceDiscoveryTimer()
{
    qDebug() << "Trying to find our Varia device...";
    this->detectNodeName();
    if (this->variaNodeName.isEmpty()) {
        qDebug() << "Varia device not found, restarting timer...";
        this->variaDeviceDiscoveryTimer->start(1000);
    }
}

void VariaConnectivity::timeoutVariaDeviceConnectionTimer()
{
    if (!this->variaDeviceConnected) {
        this->connectToVariaDevice();
    }
}

void VariaConnectivity::timeoutCycliqDeviceDiscoveryTimer()
{
    qDebug() << "Trying to find our Cycliq device...";
    this->detectNodeName();
    if (this->cycliqNodeName.isEmpty()) {
        qDebug() << "Cycliq device not found, restarting timer...";
        this->cycliqDeviceDiscoveryTimer->start(1000);
    }
}

void VariaConnectivity::timeoutCycliqDeviceConnectionTimer()
{
    if (!this->cycliqDeviceConnected) {
        this->connectToCycliqDevice();
    }
}

void VariaConnectivity::timeoutScreensaverTimer()
{
    this->disableScreensaver();
}

void VariaConnectivity::timeoutAlertsCleanupTimer()
{
    this->sentAlerts.clear();
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
            qDebug() << "Found device:" << deviceName;
            if (deviceName == "RTL64894") {
                qDebug() << "Varia device found:" << nodeName;
                emit variaDeviceDetected();
                this->variaNodeName = nodeName;
                this->variaDeviceInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + nodeName, "org.bluez.Device1", QDBusConnection::systemBus(), this);
                QStringList argumentMatch;
                argumentMatch << "org.bluez.Device1";
                QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + nodeName, "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, QString(), this, SLOT(onVariaDevicePropertiesChanged(QString, QVariantMap, QStringList)));
                this->variaDeviceConnected = this->variaDeviceInterface->property("Connected").toBool();
                this->variaServicesResolved = this->variaDeviceInterface->property("ServicesResolved").toBool();
                if (this->variaDeviceConnected) {
                    qDebug() << "Wow! Varia device already connected! :)";
                    emit variaConnectionStateChanged(this->variaDeviceConnected);
                } else {
                    qDebug() << "Varia not yet connected, trying to connect...";
                    this->variaDeviceConnectionTimer->start(2000);
                }
                if (this->variaServicesResolved) {
                    this->initializeVariaValueListeners();
                }
            }
            if (deviceName == "FLY12_951CC") {
                qDebug() << "Cycliq device found:" << nodeName;
                emit cycliqDeviceDetected();
                this->cycliqNodeName = nodeName;
                this->cycliqDeviceInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + nodeName, "org.bluez.Device1", QDBusConnection::systemBus(), this);
                QStringList argumentMatch;
                argumentMatch << "org.bluez.Device1";
                QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + nodeName, "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, QString(), this, SLOT(onVariaDevicePropertiesChanged(QString, QVariantMap, QStringList)));
                this->cycliqDeviceConnected = this->cycliqDeviceInterface->property("Connected").toBool();
                this->cycliqServicesResolved = this->cycliqDeviceInterface->property("ServicesResolved").toBool();
                if (this->cycliqDeviceConnected) {
                    qDebug() << "Wow! Cycliq device already connected! :)";
                    emit cycliqConnectionStateChanged(this->cycliqDeviceConnected);
                } else {
                    qDebug() << "Cycliq not yet connected, trying to connect...";
                    this->cycliqDeviceConnectionTimer->start(2000);
                }

                if (this->cycliqServicesResolved) {
                    this->initializeCycliqValueListeners();
                }
            }
        }
    }

    qDebug() << "Stopping device discovery...";
    QDBusMessage response = this->adapterInterface->call("StopDiscovery");
    qDebug() << "Stop Discovery response: " << response.errorMessage();
}

void VariaConnectivity::connectToVariaDevice()
{
    qDebug() << "Connecting to Varia device...";
    this->variaDeviceInterface->asyncCall("Connect");
}

void VariaConnectivity::connectToCycliqDevice()
{
    qDebug() << "Connecting to Cycliq device...";
    this->cycliqDeviceInterface->asyncCall("Connect");
}

void VariaConnectivity::initializeVariaValueListeners()
{
    qDebug() << "Initializing battery listener...";
    this->variaBatteryInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + this->variaNodeName, "org.bluez.Battery1", QDBusConnection::systemBus(), this);
    this->batteryLevel = this->variaBatteryInterface->property("Percentage").toInt();
    qDebug() << "Device battery level:" << this->batteryLevel;
    emit variaNewBatteryLevel(this->batteryLevel);
    QStringList batteryArgumentMatch;
    batteryArgumentMatch << "org.bluez.Battery1";
    QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + this->variaNodeName, "org.freedesktop.DBus.Properties", "PropertiesChanged", batteryArgumentMatch, QString(), this, SLOT(onBatteryLevelChanged(QString, QVariantMap, QStringList)));

    qDebug() << "Initializing threat listener...";
    this->variaCharacteristicInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0/" + this->variaNodeName + "/service001d/char001e", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    this->variaCharacteristicInterface->call("StartNotify");
    QStringList argumentMatch;
    argumentMatch << "org.bluez.GattCharacteristic1";
    QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + this->variaNodeName + "/service001d/char001e", "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, QString(), this, SLOT(onVariaCharacteristicPropertiesChanged(QString, QVariantMap, QStringList)));
}

void VariaConnectivity::initializeCycliqValueListeners()
{
    qDebug() << "Initialize Cycliq value listeners";
    QDBusInterface characteristic0a("org.bluez", "/org/bluez/hci0/" + this->cycliqNodeName + "/service0009/char000a", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    QDBusReply<QString> characteristic0aResponse = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 0A:" << characteristic0aResponse.value();
    QDBusMessage response0a = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 0A:" << response0a.errorMessage();

    QDBusInterface descriptor0c("org.bluez", "/org/bluez/hci0/" + this->cycliqNodeName + "/service0009/char000a/desc000c", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    QDBusReply<QString> descriptor0cResponse = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Descriptor 0C:" << descriptor0cResponse.value();
    QDBusMessage response0c = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Descriptor 0C:" << response0c.errorMessage();

    QDBusInterface characteristic0d("org.bluez", "/org/bluez/hci0/" + this->cycliqNodeName + "/service0009/char000d", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    QDBusReply<QString> characteristic0dResponse = characteristic0d.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 0D:" << characteristic0dResponse.value();
    QDBusMessage response0d = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 0D:" << response0d.errorMessage();

    QDBusInterface characteristic10("org.bluez", "/org/bluez/hci0/" + this->cycliqNodeName + "/service0009/char0010", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    QDBusReply<QString> characteristic10Response = characteristic10.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 10:" << characteristic10Response.value();
    QDBusMessage response10 = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 10:" << response10.errorMessage();

    QDBusInterface characteristic13("org.bluez", "/org/bluez/hci0/" + this->cycliqNodeName + "/service0009/char0013", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    QDBusReply<QString> characteristic13Response = characteristic13.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 13:" << characteristic13Response.value();
    QDBusMessage response13 = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 13:" << response13.errorMessage();

    QDBusInterface characteristic17("org.bluez", "/org/bluez/hci0/" + this->cycliqNodeName + "/service0009/char0017", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
    QDBusReply<QString> characteristic17Response = characteristic17.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 17:" << characteristic17Response.value();
    QDBusMessage response17 = characteristic0a.call("ReadValue", QVariantMap());
    qDebug() << "Characteristic 17:" << response17.errorMessage();

}
