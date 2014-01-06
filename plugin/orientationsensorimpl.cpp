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

#include "orientationsensorimpl.h"

#include <QDebug>

char const * const OrientationSensorImpl::id("aal.orientationsensor");

const float OrientationSensorImpl::m_accelDelta = 7.35;

OrientationSensorImpl::OrientationSensorImpl(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QOrientationReading>(&m_reading);

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
    connect(&AccelerometerCommon::instance(),
            SIGNAL(accelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)), 
            this, 
            SLOT(onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)),
            Qt::QueuedConnection);

    setDescription(QLatin1String("Orientation Sensor"));
}

OrientationSensorImpl::~OrientationSensorImpl()
{
}

void OrientationSensorImpl::start()
{
    Q_ASSERT(m_accelCommon != NULL);
    AccelerometerCommon::instance().start();
}

void OrientationSensorImpl::stop()
{
    Q_ASSERT(m_accelCommon != NULL);
    AccelerometerCommon::instance().stop();
}

void OrientationSensorImpl::onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading)
{
    // Interpret the accelerometer data into a meaningful orientation
    if (reading->y() > m_accelDelta)
        m_reading.setOrientation(QOrientationReading::TopUp);
    else if (reading->y() < -m_accelDelta)
        m_reading.setOrientation(QOrientationReading::TopDown);
    else if (reading->x() > m_accelDelta)
        m_reading.setOrientation(QOrientationReading::RightUp);
    else if (reading->x() < -m_accelDelta)
        m_reading.setOrientation(QOrientationReading::LeftUp);
    else if (reading->z() > m_accelDelta)
        m_reading.setOrientation(QOrientationReading::FaceUp);
    else if (reading->z() < -m_accelDelta)
        m_reading.setOrientation(QOrientationReading::FaceDown);

    if (m_reading.orientation() != m_readingCache.orientation())
    {
        // Emit readingChanged signal only if orientation actually changes
        newReadingAvailable();
    }

    m_readingCache.setOrientation(m_reading.orientation());
}
