/*
 * Copyright (C) 2015 Canonical, Ltd.
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

#include "core_magnetometer.h"
#include "core_shared_magnetometer.h"

#include <QDebug>

core::Magnetometer::Magnetometer(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QMagnetometerReading>(&m_reading);

    const qreal minDelay = core::SharedMagnetometer::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedMagnetometer::instance().getMinValue(),
                   core::SharedMagnetometer::instance().getMaxValue(),
                   core::SharedMagnetometer::instance().getResolution());

    // Connect to the Magnetometer's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedMagnetometer::instance(),
        SIGNAL(MagnetometerReadingChanged(QSharedPointer<QMagnetometerReading>)),
        this, 
        SLOT(onMagnetometerReadingChanged(QSharedPointer<QMagnetometerReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Magnetometer Sensor"));
}

void core::Magnetometer::start()
{
    core::SharedMagnetometer::instance().start();
}

void core::Magnetometer::stop()
{
    core::SharedMagnetometer::instance().stop();
}

void core::Magnetometer::onMagnetometerReadingChanged(QSharedPointer<QMagnetometerReading> reading)
{
    // Capture the coordinates from the Magnetometer device
    m_reading.setX(reading->x());
    m_reading.setY(reading->y());
    m_reading.setZ(reading->z());
    m_reading.setTimestamp(reading->timestamp());

    newReadingAvailable();
}
