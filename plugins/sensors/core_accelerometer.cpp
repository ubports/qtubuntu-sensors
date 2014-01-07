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

#include "core_accelerometer.h"
#include "core_shared_accelerometer.h"

#include <QDebug>

core::Accelerometer::Accelerometer(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QAccelerometerReading>(&m_reading);

    const qreal minDelay = core::SharedAccelerometer::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedAccelerometer::instance().getMinValue(),
                   core::SharedAccelerometer::instance().getMaxValue(),
                   core::SharedAccelerometer::instance().getResolution());

    // Connect to the accelerometer's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedAccelerometer::instance(),
        SIGNAL(accelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)), 
        this, 
        SLOT(onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Accelerometer Sensor"));
}

core::Accelerometer::~Accelerometer()
{
}

void core::Accelerometer::start()
{
    core::SharedAccelerometer::instance().start();
}

void core::Accelerometer::stop()
{
    core::SharedAccelerometer::instance().stop();
}

void core::Accelerometer::onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading)
{
    // Capture the coordinates from the accelerometer device
    m_reading.setX(reading->x());
    m_reading.setY(reading->y());
    m_reading.setZ(reading->z());
    m_reading.setTimestamp(reading->timestamp());

    newReadingAvailable();
}
