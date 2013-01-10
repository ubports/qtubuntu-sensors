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

#ifndef ORIENTATIONSENSOR_H
#define ORIENTATIONSENSOR_H

#include <qsensor.h>
#include <QOrientationReading>

#include <QDebug>

class OrientationSensorReadingPrivate;

class Q_DECL_EXPORT OrientationSensorReading : public QSensorReading
{
    Q_OBJECT
    DECLARE_READING(OrientationSensorReading)
};

class Q_DECL_EXPORT OrientationFilter : public QSensorFilter
{
public:
    virtual bool filter(OrientationSensorReading *reading) = 0;

private:
    bool filter(QSensorReading *reading) { return filter(static_cast<OrientationSensorReading*>(reading)); }
};

class Q_DECL_EXPORT OrientationSensor : public QSensor
{
    Q_OBJECT
public:
    explicit OrientationSensor(QObject *parent = NULL) : QSensor(OrientationSensor::type, parent) { qDebug() << __PRETTY_FUNCTION__ << endl; }
    virtual ~OrientationSensor() {}

    OrientationSensorReading *reading() const { return static_cast<OrientationSensorReading*>(QSensor::reading()); }
    static char const * const type;
};

#endif
