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

#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QVariantMap>
#include <QFile>
#include <QMutex>
#include "speedometer.h"
#include "variaconnectivity.h"

class CSVExporter : public QObject
{
    Q_OBJECT
public:
    explicit CSVExporter(Speedometer *speedometer, VariaConnectivity *variaConnectivity, QObject *parent = nullptr);
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();

signals:

private slots:
    void handleNewSpeed(qreal speed);
    void handleThreats(const QVariantList &threats);

private:
    Speedometer *speedometer;
    VariaConnectivity *variaConnectivity;
    QList<quint8> lastThreats;
    QMap<quint8, QVariantMap> recordedThreats;
    qreal currentSpeed;
    QFile *csvFile;
    QMutex threatMutex;
    bool recording = false;
    bool removalNextRound = false;
};

#endif // CSVEXPORTER_H
