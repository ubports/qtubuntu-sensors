/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Jim Hodapp <jim.hodapp@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACCELEROMETERSENSOR_H
#define ACCELEROMETERSENSOR_H

#include <qsensor.h>
#include <QAccelerometerReading>

#include <QDebug>

class AccelerometerSensorReadingPrivate;

class Q_DECL_EXPORT AccelerometerSensorReading : public QSensorReading
{
    Q_OBJECT
    DECLARE_READING(AccelerometerSensorReading)
};

class Q_DECL_EXPORT AccelerometerFilter : public QSensorFilter
{
public:
    virtual bool filter(AccelerometerSensorReading *reading) = 0;

private:
    bool filter(QSensorReading *reading) { return filter(static_cast<AccelerometerSensorReading*>(reading)); }
};

class Q_DECL_EXPORT AccelerometerSensor : public QSensor
{
public:
    explicit AccelerometerSensor(QObject *parent = NULL) : QSensor(AccelerometerSensor::type, parent) { qDebug() << __PRETTY_FUNCTION__ << endl; }
    virtual ~AccelerometerSensor() {}

    AccelerometerSensorReading *reading() const { return static_cast<AccelerometerSensorReading*>(QSensor::reading()); }
    static char const * const type;
};

#endif
