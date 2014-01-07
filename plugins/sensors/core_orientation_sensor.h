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

#ifndef CORE_ORIENTATION_SENSOR_H
#define CORE_ORIENTATION_SENSOR_H

#include <QAccelerometerReading>
#include <QOrientationReading>

#include <qsensorbackend.h>

namespace core
{
class OrientationSensor : public QSensorBackend
{
    Q_OBJECT

public:
    inline static const char* id()
    {
        return "core.orientation";
    }

    OrientationSensor(QSensor *sensor);

    // From QSensorBackend
    void start();
    void stop();

Q_SIGNALS:
    void readingChanged();

public Q_SLOTS:
    void onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading);

private:
    // The distance from the center, right or left, that will trigger an
    // orientation change when the user rotates the target device.
    static const float m_accelDelta;

    QOrientationReading m_reading;
    QOrientationReading m_readingCache;
};
}

#endif // CORE_ORIENTATION_SENSOR_H
