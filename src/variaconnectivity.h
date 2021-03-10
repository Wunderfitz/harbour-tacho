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

public slots:

private slots:
    void onDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);
    void onCharacteristicPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);

private:

    QString variaNodeName;
    bool deviceConnected;
    bool servicesResolved;
    quint8 previousIdByte;
    QVariantList previousThreats;

    QTimer *deviceDiscoveryTimer;
    QTimer *deviceConnectionTimer;
    QTimer *screensaverTimer;
    QDBusInterface *adapterInterface;
    QDBusInterface *deviceInterface;
    QDBusInterface *characteristicInterface;
    QDBusInterface *mceInterface;

    void timeoutDeviceDiscoveryTimer();
    void timeoutDeviceConnectionTimer();
    void timeoutScreensaverTimer();
    void detectNodeName();
    void connectToDevice();
    void initializeThreatListener();
};

#endif // VARIACONNECTIVITY_H
