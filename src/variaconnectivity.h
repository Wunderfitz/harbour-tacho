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
#include <QMediaPlayer>

class VariaConnectivity : public QObject
{
    Q_OBJECT
public:
    explicit VariaConnectivity(QObject *parent = nullptr);
    ~VariaConnectivity();

    Q_INVOKABLE void initializeDevices();
    Q_INVOKABLE void disableScreensaver();
    Q_INVOKABLE void enableScreensaver();

signals:
    void variaDeviceDetected();
    void cycliqDeviceDetected();
    void variaConnectionStateChanged(bool connected);
    void variaServicesResolvedStateChanged(bool variaServicesResolved);
    void cycliqConnectionStateChanged(bool connected);
    void cycliqServicesResolvedStateChanged(bool cycliqServicesResolved);
    void threatsDetected(const QVariantList &threats);
    void variaNewBatteryLevel(int batteryLevel);

public slots:

private slots:
    void onVariaDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onVariaCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onBatteryLevelChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onCycliqDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onCycliqCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);

private:

    QString variaNodeName;
    bool variaDeviceConnected;
    bool variaServicesResolved;
    QString cycliqNodeName;
    bool cycliqDeviceConnected;
    bool cycliqServicesResolved;
    quint8 previousIdByte;
    int batteryLevel;
    QVariantList previousThreats;
    QVariantList sentAlerts;

    QTimer *variaDeviceDiscoveryTimer;
    QTimer *variaDeviceConnectionTimer;
    QTimer *cycliqDeviceDiscoveryTimer;
    QTimer *cycliqDeviceConnectionTimer;
    QTimer *screensaverTimer;
    QTimer *alertsCleanupTimer;
    QDBusInterface *adapterInterface;
    QDBusInterface *variaDeviceInterface;
    QDBusInterface *cycliqDeviceInterface;
    QDBusInterface *variaBatteryInterface;
    QDBusInterface *variaCharacteristicInterface;
    QDBusInterface *mceInterface;
    QMediaPlayer *mediaPlayer;

    void timeoutVariaDeviceDiscoveryTimer();
    void timeoutVariaDeviceConnectionTimer();
    void timeoutCycliqDeviceDiscoveryTimer();
    void timeoutCycliqDeviceConnectionTimer();
    void timeoutScreensaverTimer();
    void timeoutAlertsCleanupTimer();
    void detectNodeName();
    void connectToVariaDevice();
    void connectToCycliqDevice();
    void initializeVariaValueListeners();
    void initializeCycliqValueListeners();
};

#endif // VARIACONNECTIVITY_H
