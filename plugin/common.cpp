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

#include "common.h"

#include <QDebug>

AccelerometerCommon::AccelerometerCommon(QObject *parent)
    : QObject(parent),
      m_reading(new QAccelerometerReading),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0)
{
    m_accelerometer = ua_sensors_accelerometer_new();

    ua_sensors_accelerometer_set_reading_cb(
        m_accelerometer,
        AccelerometerCommon::onAccelerometerReadingCb,
        static_cast<void *>(this)
        );
    
    // Get the minimum sensor reading delay
    m_minDelay = static_cast<qreal>(ua_sensors_accelerometer_get_min_delay(m_accelerometer));
    m_minValue = static_cast<qreal>(ua_sensors_accelerometer_get_min_value(m_accelerometer));
    m_maxValue = static_cast<qreal>(ua_sensors_accelerometer_get_max_value(m_accelerometer));
    m_resolution = static_cast<qreal>(ua_sensors_accelerometer_get_resolution(m_accelerometer));
}

AccelerometerCommon::~AccelerometerCommon()
{
    if (m_reading != NULL) {
        delete m_reading;
    }
}

void AccelerometerCommon::start()
{
    ua_sensors_accelerometer_enable(m_accelerometer);
}

void AccelerometerCommon::stop()
{
    ua_sensors_accelerometer_disable(m_accelerometer);
}

QAccelerometerReading *AccelerometerCommon::reading() const
{
    return m_reading;
}

qreal AccelerometerCommon::getMinDelay() const
{
    return m_minDelay;
}

qreal AccelerometerCommon::getMinValue() const
{
    return m_minValue;
}

qreal AccelerometerCommon::getMaxValue() const
{
    return m_maxValue;
}

qreal AccelerometerCommon::getResolution() const
{
    return m_resolution;
}

void AccelerometerCommon::onAccelerometerReadingCb(UASAccelerometerEvent *event, void *context)
{
    AccelerometerCommon *ac = static_cast<AccelerometerCommon *>(context);
    if (ac != NULL)
        ac->onAccelerometerReading(event);
}

void AccelerometerCommon::onAccelerometerReading(UASAccelerometerEvent *event)
{
    Q_ASSERT(event != NULL);

    float accel_x = uas_accelerometer_event_get_acceleration_x(event);
    float accel_y = uas_accelerometer_event_get_acceleration_y(event);
    float accel_z = uas_accelerometer_event_get_acceleration_z(event);

    // Check for duplicate values to avoid unnecessary signal emission
    if (m_reading->x() == accel_x &&
        m_reading->y() == accel_y &&
        m_reading->z() == accel_z)
    {
        return;
    }

    // Capture the coordinates from the accelerometer device
    m_reading->setX(accel_x);
    m_reading->setY(accel_y);
    m_reading->setZ(accel_z);

    // Set the timestamp as set by the hybris layer
    m_reading->setTimestamp(uas_accelerometer_event_get_timestamp(event));

    Q_EMIT accelerometerReadingChanged();
}
