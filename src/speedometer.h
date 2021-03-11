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
#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <QObject>
#include <QGeoPositionInfoSource>

class Speedometer : public QObject
{
    Q_OBJECT
public:
    explicit Speedometer(QObject *parent = nullptr);
    ~Speedometer();

signals:
    void newSpeed(qreal speed);

private slots:
    void handleGeoPositionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *geoPositionInfoSource;

};

#endif // SPEEDOMETER_H
