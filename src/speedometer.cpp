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
#include "speedometer.h"
#include <QDebug>
#include <QGeoPositionInfo>

Speedometer::Speedometer(QObject *parent) : QObject(parent)
{
    qDebug() << "Initializing speedometer";
    this->geoPositionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);
    if (this->geoPositionInfoSource) {
        qDebug() << "Geolocation successfully initialized...";
        this->geoPositionInfoSource->setUpdateInterval(1000);
        connect(geoPositionInfoSource, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(handleGeoPositionUpdated(QGeoPositionInfo)));
        this->geoPositionInfoSource->startUpdates();
    } else {
        qDebug() << "Unable to initialize geolocation!" ;
    }
}

Speedometer::~Speedometer()
{
    qDebug() << "Shutting down speedometer";
    if (this->geoPositionInfoSource) {
        this->geoPositionInfoSource->stopUpdates();
    }
}

void Speedometer::handleGeoPositionUpdated(const QGeoPositionInfo &info)
{
    qDebug() << "New geo positioning info";
    qreal groundSpeed = info.attribute(QGeoPositionInfo::GroundSpeed);
    qDebug() << "Ground Speed:" << groundSpeed;
    qreal horizontalAccuracy = info.attribute(QGeoPositionInfo::HorizontalAccuracy);
    qDebug() << "Horizontal Accuracy:" << horizontalAccuracy;
    if (horizontalAccuracy < 100 && groundSpeed > 0) {
        emit newSpeed(groundSpeed * 3.6);
    } else {
        emit newSpeed(0);
    }
    QGeoCoordinate coordinate = info.coordinate();
    emit newPosition(coordinate.latitude(), coordinate.longitude(), horizontalAccuracy);
}
