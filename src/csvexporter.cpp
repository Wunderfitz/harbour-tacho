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

#include "csvexporter.h"

#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QTextStream>

CSVExporter::CSVExporter(Speedometer *speedometer, VariaConnectivity *variaConnectivity, QObject *parent) : QObject(parent)
{
    this->variaConnectivity = variaConnectivity;
    this->speedometer = speedometer;

    connect(this->speedometer, SIGNAL(newSpeed(qreal)), this, SLOT(handleNewSpeed(qreal)));
    connect(this->variaConnectivity, SIGNAL(threatsDetected(QVariantList)), this, SLOT(handleThreats(QVariantList)));
}

void CSVExporter::startRecording()
{
    qDebug() << "CSV Exporter - Start recording";
    QString currently = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
    QString csvFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/tacho-" + currently + ".csv";
    this->csvFile = new QFile(csvFilePath);
        qDebug() << "Creating CSV recording file at " << this->csvFile->fileName();
        if (this->csvFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream fileOut(this->csvFile);
            fileOut.setCodec("UTF-8");
            fileOut << QString("Threat ID;First Seen;Last Seen;Last Vehicle Speed;Last Vehicle Distance;My Speed;").toUtf8() << "\n";
            fileOut.flush();
        }
}

void CSVExporter::stopRecording()
{
    qDebug() << "CSV Exporter - Stop recording";
    this->csvFile->close();
    this->csvFile->deleteLater();
}

void CSVExporter::handleNewSpeed(qreal speed)
{
    qDebug() << "CSV Exporter - New speed: " << speed;
    this->currentSpeed = speed;
}

void CSVExporter::handleThreats(const QVariantList &threats)
{
    this->threatMutex.lock();
    QList<quint8> currentThreats;
    QString currently = QDateTime::currentDateTime().toString(Qt::ISODate);
    for (QVariant rawThreat : threats) {
        QVariantMap threat = rawThreat.toMap();
        quint8 threatNumber = threat.value("number").toInt();
        QVariantMap recordedThreat;
        if (this->threats.contains(threatNumber)) {
            recordedThreat = this->threats.value(threatNumber);
        } else {
            recordedThreat.insert("firstSeen", currently);
        }
        recordedThreat.insert("lastSeen", currently);
        recordedThreat.insert("vehicleSpeed", threat.value("speed").toInt());
        recordedThreat.insert("vehicleDistance", threat.value("distance").toInt());
        recordedThreat.insert("mySpeed", this->currentSpeed);
        this->threats.insert(threatNumber, recordedThreat);
        currentThreats.append(threatNumber);
    }
    for (quint8 lastThreat : this->lastThreats) {
        if (!currentThreats.contains(lastThreat)) {
            QVariantMap goneThreat = this->threats.value(lastThreat);
            QTextStream fileOut(this->csvFile);
            fileOut.setCodec("UTF-8");
            fileOut << QString::number(lastThreat) << ";";
            fileOut << goneThreat.value("firstSeen").toString() << ";";
            fileOut << goneThreat.value("lastSeen").toString() << ";";
            fileOut << goneThreat.value("vehicleSpeed").toString() << ";";
            fileOut << goneThreat.value("vehicleDistance").toString() << ";";
            fileOut << goneThreat.value("mySpeed").toString() << ";";
            fileOut << "\n";
            fileOut.flush();
            this->threats.remove(lastThreat);
        }
    }
    this->lastThreats = currentThreats;
    this->threatMutex.unlock();
}
