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

#ifndef CORE_SHARED_ACCELEROMETER_H
#define CORE_SHARED_ACCELEROMETER_H

#include <ubuntu/application/sensors/accelerometer.h>

#include <QAccelerometerReading>
#include <QObject>
#include <QSharedPointer>

Q_DECLARE_METATYPE(QSharedPointer<QAccelerometerReading>)

namespace core
{
class SharedAccelerometer : public QObject
{
    Q_OBJECT

public:
    static SharedAccelerometer& instance();

    void start();
    void stop();

    qreal getMinDelay() const;
    qreal getMinValue() const;
    qreal getMaxValue() const;
    qreal getResolution() const;

Q_SIGNALS:
    void accelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading);

private:
    SharedAccelerometer(QObject *parent = NULL);

    UASensorsAccelerometer *m_accelerometer;

    qreal m_minDelay;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_resolution;

    // Gets called by the underlying platform when there is a new accelerometer reading
    static void onAccelerometerReadingCb(UASAccelerometerEvent *event, void *context);
    void onAccelerometerReading(UASAccelerometerEvent *event);
};
}

#endif // CORE_SHARED_ACCELEROMETER_H
