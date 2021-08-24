/*
 * Copyright (C) 2020 UBports Foundation
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
 *
 * Authored by: Florian Leeber <florian@ubports.com>
 */

#include "core_pressure.h"
#include "core_shared_pressure.h"

#include <QDebug>

core::Pressure::Pressure(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QPressureReading>(&m_reading);

    const qreal minDelay = core::SharedPressure::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedPressure::instance().getMinValue(),
                   core::SharedPressure::instance().getMaxValue(),
                   core::SharedPressure::instance().getResolution());

    // Connect to the Pressure's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedPressure::instance(),
        SIGNAL(pressureReadingChanged(QSharedPointer<QPressureReading>)),
        this, 
        SLOT(onPressureReadingChanged(QSharedPointer<QPressureReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Pressure Sensor"));
}

void core::Pressure::start()
{
    core::SharedPressure::instance().start();
}

void core::Pressure::stop()
{
    core::SharedPressure::instance().stop();
}

void core::Pressure::onPressureReadingChanged(QSharedPointer<QPressureReading> reading)
{
    // Capture the coordinates from the Pressure device
    m_reading.setPressure(reading->pressure());
    m_reading.setTimestamp(reading->timestamp());

    newReadingAvailable();
}

