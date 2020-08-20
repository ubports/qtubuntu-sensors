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

#include "core_light.h"
#include "core_shared_light.h"

#include <QDebug>

core::Light::Light(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QLightReading>(&m_reading);

    const qreal minDelay = core::SharedLight::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedLight::instance().getMinValue(),
                   core::SharedLight::instance().getMaxValue(),
                   core::SharedLight::instance().getResolution());

    // Connect to the Light's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedLight::instance(),
        SIGNAL(lightReadingChanged(QSharedPointer<QLightReading>)),
        this, 
        SLOT(onLightReadingChanged(QSharedPointer<QLightReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Light Sensor"));
}

void core::Light::start()
{
    core::SharedLight::instance().start();
}

void core::Light::stop()
{
    core::SharedLight::instance().stop();
}

void core::Light::onLightReadingChanged(QSharedPointer<QLightReading> reading)
{
    // Capture the coordinates from the Light device
    m_reading.setLux(reading->lux());
    m_reading.setTimestamp(reading->timestamp());

    newReadingAvailable();
}

