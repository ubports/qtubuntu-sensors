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
      m_reading(new QAccelerometerReading)
{
    ubuntu_sensor_initialize_observer(&m_observer);
    m_observer.on_new_accelerometer_reading_cb = AccelerometerCommon::onAccelerometerReadingCb;
    m_observer.context = static_cast<void *>(this);
}

AccelerometerCommon::~AccelerometerCommon()
{
    if (m_reading != NULL) {
        delete m_reading;
    }
}

void AccelerometerCommon::start()
{
    ubuntu_sensor_install_observer(&m_observer);
    ubuntu_sensor_enable_sensor(ubuntu_sensor_type_accelerometer);
}

void AccelerometerCommon::stop()
{
    ubuntu_sensor_disable_sensor(ubuntu_sensor_type_accelerometer);
}

QAccelerometerReading *AccelerometerCommon::reading() const
{
    return m_reading;
}

void AccelerometerCommon::onAccelerometerReadingCb(ubuntu_sensor_accelerometer_reading *reading, void *context)
{
    AccelerometerCommon *ac = static_cast<AccelerometerCommon *>(context);
    if (ac != NULL)
        ac->onAccelerometerReading(reading);
}

void AccelerometerCommon::onAccelerometerReading(ubuntu_sensor_accelerometer_reading *reading)
{
    Q_ASSERT(reading != NULL);

    // Check for duplicate values to avoid unnecessary signal emission
    if (m_reading->x() == reading->acceleration_x &&
        m_reading->y() == reading->acceleration_y &&
        m_reading->z() == reading->acceleration_z)
    {
        return;
    }

    // Capture the coordinates from the accelerometer device
    m_reading->setX(reading->acceleration_x);
    m_reading->setY(reading->acceleration_y);
    m_reading->setZ(reading->acceleration_z);

    // Set the timestamp as set by the hybris layer
    m_reading->setTimestamp(reading->timestamp);

    Q_EMIT accelerometerReadingChanged();
}
