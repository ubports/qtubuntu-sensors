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

#ifndef CORE_ACCELEROMETER_H
#define CORE_ACCELEROMETER_H

#include <QAccelerometerReading>

#include <qsensorbackend.h>

namespace core
{
class Accelerometer : public QSensorBackend
{
    Q_OBJECT
public:
    inline static const char* id()
    {
        return "core.accelerometer";
    }

    Accelerometer(QSensor *sensor);
    virtual ~Accelerometer();

    void start();
    void stop();

public Q_SLOTS:
    void onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading);

private:
    QAccelerometerReading m_reading;
};
}

#endif // CORE_ACCELEROMETER_H
