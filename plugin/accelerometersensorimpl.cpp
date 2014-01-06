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

#include "accelerometersensorimpl.h"

#include <QDebug>

char const * const AccelerometerSensorImpl::id("aal.accelerometer");

AccelerometerSensorImpl::AccelerometerSensorImpl(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QAccelerometerReading>(&m_reading);

    const qreal minDelay = AccelerometerCommon::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(AccelerometerCommon::instance().getMinValue(),
                   AccelerometerCommon::instance().getMaxValue(),
                   AccelerometerCommon::instance().getResolution());

    // Connect to the accelerometer's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &AccelerometerCommon::instance(), 
        SIGNAL(accelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)), 
        this, 
        SLOT(onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Accelerometer Sensor"));
}

AccelerometerSensorImpl::~AccelerometerSensorImpl()
{
}

void AccelerometerSensorImpl::start()
{
    AccelerometerCommon::instance().start();
}

void AccelerometerSensorImpl::stop()
{
    AccelerometerCommon::instance().stop();
}

void AccelerometerSensorImpl::onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading)
{
    printf("%s: %p: ", __PRETTY_FUNCTION__, this);

    // Capture the coordinates from the accelerometer device
    m_reading.setX(reading->x());
    m_reading.setY(reading->y());
    m_reading.setZ(reading->z());
    m_reading.setTimestamp(reading->timestamp());
    
    printf("%f, %f, %f \n", reading->x(), reading->y(), reading->z());

    newReadingAvailable();
}
