#ifndef VARIACONNECTIVITY_H
#define VARIACONNECTIVITY_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

class VariaConnectivity : public QObject
{
    Q_OBJECT
public:
    explicit VariaConnectivity(QObject *parent = nullptr);

    Q_INVOKABLE void initializeRadar();

signals:
    void threatsDetected(const QVariantList &threats);

public slots:

private slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &map, const QStringList &list);

private:
    QString nodeName;
    QVariantMap threats;
};

#endif // VARIACONNECTIVITY_H
