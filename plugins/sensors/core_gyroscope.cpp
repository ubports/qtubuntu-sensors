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

#include "core_gyroscope.h"
#include "core_shared_gyroscope.h"

#include <QDebug>

core::Gyroscope::Gyroscope(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QGyroscopeReading>(&m_reading);

    const qreal minDelay = core::SharedGyroscope::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedGyroscope::instance().getMinValue(),
                   core::SharedGyroscope::instance().getMaxValue(),
                   core::SharedGyroscope::instance().getResolution());

    // Connect to the Gyroscope's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedGyroscope::instance(),
        SIGNAL(gyroscopeReadingChanged(QSharedPointer<QGyroscopeReading>)),
        this, 
        SLOT(onGyroscopeReadingChanged(QSharedPointer<QGyroscopeReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Gyroscope Sensor"));
}

void core::Gyroscope::start()
{
    core::SharedGyroscope::instance().start();
}

void core::Gyroscope::stop()
{
    core::SharedGyroscope::instance().stop();
}

void core::Gyroscope::onGyroscopeReadingChanged(QSharedPointer<QGyroscopeReading> reading)
{
    // Capture the coordinates from the Gyroscope device
    m_reading.setX(reading->x());
    m_reading.setY(reading->y());
    m_reading.setZ(reading->z());
    m_reading.setTimestamp(reading->timestamp());

    newReadingAvailable();
}
