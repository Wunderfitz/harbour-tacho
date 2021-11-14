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
    connect(this->speedometer, SIGNAL(newPosition(double, double, qreal)), this, SLOT(handleNewPosition(double, double, qreal)));
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
            fileOut << QString("Threat ID;First Seen;Last Seen;Last Vehicle Relative Speed (km/h);Last Vehicle Distance (m);My Speed (km/h);Latitude;Longitude;Accuracy (m)").toUtf8() << "\n";
            fileOut.flush();
            this->recording = true;
        }
}

void CSVExporter::stopRecording()
{
    qDebug() << "CSV Exporter - Stop recording";
    this->recording = false;
    this->csvFile->close();
    this->csvFile->deleteLater();
}

void CSVExporter::handleNewSpeed(qreal speed)
{
    qDebug() << "CSV Exporter - New speed: " << speed;
    this->currentSpeed = speed;
}

void CSVExporter::handleNewPosition(double latitude, double longitude, qreal accuracy)
{
    qDebug() << "CSV Exporter - New position: " << latitude << longitude << accuracy;
    this->latitude = latitude;
    this->longitude = longitude;
    this->accuracy = accuracy;
}

void CSVExporter::handleThreats(const QVariantList &threats)
{
    if (!this->recording) {
        return;
    }
    this->threatMutex.lock();
    QList<quint8> currentThreats;
    QString currently = QDateTime::currentDateTime().toString(Qt::ISODate);
    for (QVariant rawThreat : threats) {
        QVariantMap threat = rawThreat.toMap();
        quint8 threatNumber = threat.value("number").toInt();
        QVariantMap recordedThreat;
        if (this->recordedThreats.contains(threatNumber)) {
            recordedThreat = this->recordedThreats.value(threatNumber);
        } else {
            recordedThreat.insert("firstSeen", currently);
        }
        recordedThreat.insert("lastSeen", currently);
        recordedThreat.insert("vehicleSpeed", threat.value("speed").toInt());
        recordedThreat.insert("vehicleDistance", threat.value("distance").toInt());
        recordedThreat.insert("mySpeed", this->currentSpeed);
        recordedThreat.insert("latitude", this->latitude);
        recordedThreat.insert("longitude", this->longitude);
        recordedThreat.insert("accuracy", this->accuracy);
        this->recordedThreats.insert(threatNumber, recordedThreat);
        currentThreats.append(threatNumber);
    }
    if (this->removalNextRound) {
        qDebug() << "Current Threats:" << currentThreats;
        qDebug() << "Last Threats:   " << this->lastThreats;
        for (quint8 lastThreat : this->lastThreats) {
            if (!currentThreats.contains(lastThreat)) {
                qDebug() << "Threat" << lastThreat << "is gone, exporting information...";
                QVariantMap goneThreat = this->recordedThreats.value(lastThreat);
                QTextStream fileOut(this->csvFile);
                fileOut.setCodec("UTF-8");
                fileOut << QString::number(lastThreat) << ";";
                fileOut << goneThreat.value("firstSeen").toString() << ";";
                fileOut << goneThreat.value("lastSeen").toString() << ";";
                fileOut << goneThreat.value("vehicleSpeed").toString() << ";";
                fileOut << goneThreat.value("vehicleDistance").toString() << ";";
                fileOut << goneThreat.value("mySpeed").toString() << ";";
                fileOut << goneThreat.value("latitude").toString() << ";";
                fileOut << goneThreat.value("longitude").toString() << ";";
                fileOut << goneThreat.value("accuracy").toString() << ";";
                fileOut << "\n";
                fileOut.flush();
                this->recordedThreats.remove(lastThreat);
            }
        }
        this->lastThreats.clear();
        this->lastThreats.append(currentThreats);
        this->removalNextRound = false;
    } else {
        this->removalNextRound = true;
    }
    this->threatMutex.unlock();
}
