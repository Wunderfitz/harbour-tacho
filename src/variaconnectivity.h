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
#ifndef VARIACONNECTIVITY_H
#define VARIACONNECTIVITY_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QTimer>
#include <QDBusInterface>

class VariaConnectivity : public QObject
{
    Q_OBJECT
public:
    explicit VariaConnectivity(QObject *parent = nullptr);
    ~VariaConnectivity();

    Q_INVOKABLE void initializeRadar();
    Q_INVOKABLE void disableScreensaver();
    Q_INVOKABLE void enableScreensaver();

signals:
    void deviceDetected();
    void connectionStateChanged(bool connected);
    void servicesResolvedStateChanged(bool servicesResolved);
    void threatsDetected(const QVariantList &threats);
    void newBatteryLevel(int batteryLevel);

public slots:

private slots:
    void onDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onBatteryLevelChanged(const QString &interface, const QVariantMap &map, const QStringList &list);

private:

    QString variaNodeName;
    bool deviceConnected;
    bool servicesResolved;
    quint8 previousIdByte;
    int batteryLevel;
    QVariantList previousThreats;

    QTimer *deviceDiscoveryTimer;
    QTimer *deviceConnectionTimer;
    QTimer *screensaverTimer;
    QDBusInterface *adapterInterface;
    QDBusInterface *deviceInterface;
    QDBusInterface *batteryInterface;
    QDBusInterface *characteristicInterface;
    QDBusInterface *mceInterface;

    void timeoutDeviceDiscoveryTimer();
    void timeoutDeviceConnectionTimer();
    void timeoutScreensaverTimer();
    void detectNodeName();
    void connectToDevice();
    void initializeValueListeners();
};

#endif // VARIACONNECTIVITY_H
