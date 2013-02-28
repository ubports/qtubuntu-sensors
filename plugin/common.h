/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

#include <ubuntu/application/sensors/ubuntu_application_sensors.h>

#include <QAccelerometerReading>
#include <QObject>

class AccelerometerCommon : public QObject
{
    Q_OBJECT

public:
    AccelerometerCommon(QObject *parent = NULL);
    ~AccelerometerCommon();

    void start();
    void stop();

    QAccelerometerReading *reading() const;

    qreal getMinDelay() const;
    qreal getMinValue() const;
    qreal getMaxValue() const;
    qreal getResolution() const;

    static void onAccelerometerReadingCb(ubuntu_sensor_accelerometer_reading *reading, void *context);

Q_SIGNALS:
    void accelerometerReadingChanged();

private:
    ubuntu_sensor_observer m_observer;
    QAccelerometerReading *m_reading;
    qreal m_minDelay;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_resolution;

    // Gets called by the Aal sensor wrapper when there is a new accelerometer reading
    void onAccelerometerReading(ubuntu_sensor_accelerometer_reading *reading);
};

#endif
