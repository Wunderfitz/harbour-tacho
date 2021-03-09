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

    Q_INVOKABLE void initializeRadar();

signals:
    void deviceDetected();
    void connectionStateChanged(bool connected);
    void servicesResolvedStateChanged(bool servicesResolved);
    void threatsDetected(const QVariantList &threats);

public slots:

private slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);

    void onDevicePropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);

private:

    QVariantMap threats;

    QString variaNodeName;
    bool deviceConnected;
    bool servicesResolved;

    QTimer *deviceDiscoveryTimer;
    QTimer *deviceConnectionTimer;
    QDBusInterface *adapterInterface;
    QDBusInterface *deviceInterface;

    void timeoutDeviceDiscoveryTimer();
    void timeoutDeviceConnectionTimer();
    void detectNodeName();
    void connectToDevice();
};

#endif // VARIACONNECTIVITY_H
