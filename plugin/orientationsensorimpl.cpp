/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Jim Hodapp <jim.hodapp@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "orientationsensorimpl.h"

#include <QDebug>

char const * const OrientationSensorImpl::id("aal.orientationsensor");

const float OrientationSensorImpl::m_accelDelta = 7.35;

OrientationSensorImpl::OrientationSensorImpl(QSensor *sensor)
    : QSensorBackend(sensor)
{
    m_accelCommon = new AccelerometerCommon(this);

    // Register the reading instance with the parent
    setReading<QOrientationReading>(&m_reading);

    // Connect to the accelerometer's readingChanged signal
    connect(m_accelCommon, SIGNAL(accelerometerReadingChanged()), this, SLOT(onAccelerometerReadingChanged()));
    // TODO: add to hybris wrapper, then enable here:
    //setDataRates(tbd);
    //addOutputRange(0, 9.8, 0.1);

    setDescription(QLatin1String("Orientation Sensor"));
}

OrientationSensorImpl::~OrientationSensorImpl()
{
    if (m_accelCommon != NULL) {
        delete m_accelCommon;
    }
}

void OrientationSensorImpl::start()
{
    Q_ASSERT(m_accelCommon != NULL);
    m_accelCommon->start();
}

void OrientationSensorImpl::stop()
{
    Q_ASSERT(m_accelCommon != NULL);
    m_accelCommon->stop();
}

void OrientationSensorImpl::onAccelerometerReadingChanged()
{
    Q_ASSERT(m_accelCommon != NULL);

    const QAccelerometerReading *reading = m_accelCommon->reading();
    Q_ASSERT(reading != NULL);

    m_reading.setTimestamp(AccelerometerCommon::getTimeStamp());

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

    // Emit readingChanged signal
    newReadingAvailable();
}
