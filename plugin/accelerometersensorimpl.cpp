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
    m_accelCommon = new AccelerometerCommon(this);

    // Register the reading instance with the parent
    setReading<QAccelerometerReading>(&m_reading);

    // Connect to the accelerometer's readingChanged signal
    connect(m_accelCommon, SIGNAL(accelerometerReadingChanged()), this, SLOT(onAccelerometerReadingChanged()));

    // TODO: add to hybris wrapper, then enable here:
    //addDataRate(100, 100); // 100Hz

    setDescription(QLatin1String("Accelerometer Sensor"));
}

AccelerometerSensorImpl::~AccelerometerSensorImpl()
{
    if (m_accelCommon != NULL) {
        delete m_accelCommon;
    }
}

void AccelerometerSensorImpl::start()
{
    Q_ASSERT(m_accelCommon != NULL);
    m_accelCommon->start();
}

void AccelerometerSensorImpl::stop()
{
    Q_ASSERT(m_accelCommon != NULL);
    m_accelCommon->stop();
}

void AccelerometerSensorImpl::onAccelerometerReadingChanged()
{
    Q_ASSERT(m_accelCommon != NULL);

    const QAccelerometerReading *reading = m_accelCommon->reading();
    Q_ASSERT(reading != NULL);

    // Capture the coordinates from the accelerometer device
    m_reading.setX(reading->x());
    m_reading.setY(reading->y());
    m_reading.setZ(reading->z());

    newReadingAvailable();
}
