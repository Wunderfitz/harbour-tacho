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

VariaConnectivity::VariaConnectivity(QObject *parent) : QObject(parent)
{
    this->deviceDiscoveryTimer = new QTimer(this);
    this->deviceDiscoveryTimer->setSingleShot(true);
    connect(deviceDiscoveryTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutDeviceDiscoveryTimer);

    this->deviceConnectionTimer = new QTimer(this);
    connect(deviceConnectionTimer, &QTimer::timeout, this, &VariaConnectivity::timeoutDeviceConnectionTimer);
}

void VariaConnectivity::initializeRadar()
{
    qDebug() << "Initializing Varia Connectivity...";
    this->adapterInterface = new QDBusInterface("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
    qDebug() << "Bluetooth Device Alias:" << this->adapterInterface->property("Alias");
    QDBusMessage response = this->adapterInterface->call("StartDiscovery");
    qDebug() << "Start Discovery response: " << response.errorMessage();
    this->deviceDiscoveryTimer->start(1000);

//    QDBusInterface introspectableInterface("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Introspectable", QDBusConnection::systemBus(), this);
//    QDBusReply<QString> introspectResponse = introspectableInterface.call("Introspect");

//    QDomDocument introspectDocument;
//    introspectDocument.setContent(introspectResponse.value());
//    QDomNodeList foundNodes = introspectDocument.elementsByTagName("node");
//    for (int i = 0; i < foundNodes.count(); i++) {
//        QDomElement foundElement = foundNodes.at(i).toElement();
//        nodeName = foundElement.attribute("name");

//        QStringList argumentMatch;
//        argumentMatch << "org.bluez.GattCharacteristic1";
//        QDBusConnection::systemBus().connect("org.bluez", "/org/bluez/hci0/" + nodeName + "/service001d/char001e", "org.freedesktop.DBus.Properties","PropertiesChanged", argumentMatch, QString(),
//                                this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

//        if (nodeName.startsWith("dev_")) {
//            QDBusInterface deviceInterface("org.bluez", "/org/bluez/hci0/" + nodeName, "org.bluez.Device1", QDBusConnection::systemBus(), this);
//            QString deviceName = deviceInterface.property("Name").toString();
//            if (deviceName == "RTL64894") {
//                qDebug() << "Detected device:" << deviceName;
//                qDebug() << "Address:" << deviceInterface.property("Address").toString();
//                qDebug() << "Services resolved:" << deviceInterface.property("ServicesResolved").toString();
//                QDBusInterface serviceInterface1("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000a", "org.bluez.GattService1", QDBusConnection::systemBus(), this);
//                qDebug() << "Service 1 UUID:" << serviceInterface1.property("UUID").toString();
//                QDBusInterface characteristicInterface11("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000a/char000b", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                qDebug() << "Characteristic 1-1 UUID:" << characteristicInterface11.property("UUID").toString();
//                QDBusReply<QByteArray> characteristicResponse = characteristicInterface11.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 1-1 error?" << characteristicResponse.error().message();
//                qDebug() << "Characteristic 1-1 value:" << characteristicResponse.value();
//                QDBusInterface descriptorInterface11("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000a/char000b/desc000d", "org.bluez.GattDescriptor1", QDBusConnection::systemBus(), this);
//                qDebug() << "Descriptor 1-1 UUID:" << descriptorInterface11.property("UUID").toString();
//                QDBusReply<QByteArray> descriptorResponse11 = descriptorInterface11.call("ReadValue", QVariantMap());
//                qDebug() << "Descriptor 1-1 value:" << descriptorResponse11.value();




//                QDBusInterface serviceInterface2("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000e", "org.bluez.GattService1", QDBusConnection::systemBus(), this);
//                qDebug() << "Service 2 UUID:" << serviceInterface2.property("UUID").toString();

//                QDBusInterface characteristicInterface21("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000e/char000f", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                QDBusReply<QByteArray> characteristicResponse21 = characteristicInterface21.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 2-1 error?" << characteristicResponse21.error().message();
//                qDebug() << "Characteristic 2-1 value" << characteristicResponse21.value();

//                QDBusInterface characteristicInterface22("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000e/char0011", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                QDBusReply<QByteArray> characteristicResponse22 = characteristicInterface22.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 2-2 error?" << characteristicResponse22.error().message();
//                qDebug() << "Characteristic 2-2 value" << characteristicResponse22.value();

//                QDBusInterface characteristicInterface23("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000e/char0013", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                QDBusReply<QByteArray> characteristicResponse23 = characteristicInterface23.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 2-3 error?" << characteristicResponse23.error().message();
//                qDebug() << "Characteristic 2-3 value" << characteristicResponse23.value();

//                QDBusInterface characteristicInterface24("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000e/char0015", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                QDBusReply<QByteArray> characteristicResponse24 = characteristicInterface24.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 2-4 error?" << characteristicResponse24.error().message();
//                qDebug() << "Characteristic 2-4 value" << characteristicResponse24.value();

//                QDBusInterface characteristicInterface25("org.bluez", "/org/bluez/hci0/" + nodeName + "/service000e/char0017", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                QDBusReply<QByteArray> characteristicResponse25 = characteristicInterface25.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 2-5 error?" << characteristicResponse25.error().message();
//                qDebug() << "Characteristic 2-5 value" << characteristicResponse25.value();

//                QDBusInterface serviceInterface3("org.bluez", "/org/bluez/hci0/" + nodeName + "/service001d", "org.bluez.GattService1", QDBusConnection::systemBus(), this);
//                qDebug() << "Service 3 UUID:" << serviceInterface3.property("UUID").toString();
//                QDBusInterface characteristicInterface31("org.bluez", "/org/bluez/hci0/" + nodeName + "/service001d/char001e", "org.bluez.GattCharacteristic1", QDBusConnection::systemBus(), this);
//                characteristicInterface31.call("StartNotify");

//                QDBusReply<QByteArray> characteristicResponse31 = characteristicInterface31.call("ReadValue", QVariantMap());
//                qDebug() << "Characteristic 3-1 error?" << characteristicResponse31.error().message();
//                qDebug() << "Characteristic 3-1 value" << characteristicResponse31.value();
//                QByteArray characteristicByteArray = characteristicResponse31.value();
//                qDebug() << "Response size: " << characteristicByteArray.count();

//                qDebug() << "Piep!";
//            }
//        }
//    }
}

void VariaConnectivity::onPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    Q_UNUSED(interface);
    Q_UNUSED(list);
    //qDebug() << "Characteristic property changed" << interface << map;
    QVariantList currentThreats;

    if (map.contains("Value")) {
        QByteArray radarData = map["Value"].toByteArray();
        qDebug() << "-----------------------------------------------------------------------------";
        qDebug() << "Data Length:" << radarData.length();
        int currentMilliseconds = QTime::currentTime().msecsSinceStartOfDay();
        int foundThreats = ( radarData.length() - 1 ) / 3;
        qDebug() << "Threats found:" << foundThreats;
        quint8 idByte = radarData.at(0);
        for (int i = 0; i < foundThreats; i++) {
            quint8 byte1 = radarData.at(1 + (i * 3));
            quint8 threatDistance = radarData.at(2 + (i * 3));
            quint8 byte3 = radarData.at(3 + (i * 3));
            qDebug() << "ID:" << idByte << "Index:" << QString::number(i + 1) << "Byte 1:" << byte1 << "Threat Distance:" << threatDistance << "Byte 3:" << byte3 << "milliseconds" << currentMilliseconds;
            QVariantMap currentThreat;
            currentThreat.insert("threatId", byte1);
            currentThreat.insert("distance", threatDistance);
            currentThreat.insert("speed", byte3);
            currentThreats.append(currentThreat);

        }
        qDebug() << "-----------------------------------------------------------------------------";
        qDebug() << "Value changed!!" << map["Value"].toByteArray();
    }
    emit threatsDetected(currentThreats);
}

void VariaConnectivity::onDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list)
{
    qDebug() << "Device properties changed" << interface << map << list;
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
        }
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
