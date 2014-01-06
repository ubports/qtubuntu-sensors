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
#include <QMetaType>

AccelerometerCommon& AccelerometerCommon::instance()
{
    static AccelerometerCommon instance;
    return instance;
}

AccelerometerCommon::AccelerometerCommon(QObject *parent)
    : QObject(parent),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0)
{
    qRegisterMetaType<QSharedPointer<QAccelerometerReading> >("QSharedPointer<QAccelerometerReading>");
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
}

void AccelerometerCommon::start()
{
    ua_sensors_accelerometer_enable(m_accelerometer);
}

void AccelerometerCommon::stop()
{
    ua_sensors_accelerometer_disable(m_accelerometer);
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
    printf("%s: %p: ", __PRETTY_FUNCTION__, this);
    Q_ASSERT(event != NULL);

    QSharedPointer<QAccelerometerReading> reading(new QAccelerometerReading());
    reading->setX(uas_accelerometer_event_get_acceleration_x(event));
    reading->setY(uas_accelerometer_event_get_acceleration_y(event));
    reading->setZ(uas_accelerometer_event_get_acceleration_z(event));

    printf("%f, %f, %f \n", reading->x(), reading->y(), reading->z());

    // Set the timestamp as set by the hybris layer
    reading->setTimestamp(uas_accelerometer_event_get_timestamp(event));

    Q_EMIT accelerometerReadingChanged(reading);
}
