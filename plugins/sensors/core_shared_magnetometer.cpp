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

#include "core_shared_magnetometer.h"

#include <QDebug>
#include <QMetaType>

Q_DECLARE_METATYPE(QSharedPointer<QMagnetometerReading>)

core::SharedMagnetometer& core::SharedMagnetometer::instance()
{
    static core::SharedMagnetometer instance;
    return instance;
}

core::SharedMagnetometer::SharedMagnetometer(QObject *parent)
    : QObject(parent),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0),
      m_available(false)
{
    qRegisterMetaType<QSharedPointer<QMagnetometerReading> >("QSharedPointer<QMagnetometerReading>");
    m_magnetometer = ua_sensors_magnetic_new();
    if (m_magnetometer == NULL)
        return;

    ua_sensors_magnetic_set_reading_cb(
                m_magnetometer,
                core::SharedMagnetometer::onMagnetometerReadingCb,
                static_cast<void *>(this));

    // Get the minimum sensor reading delay
    m_minDelay = ua_sensors_magnetic_get_min_delay(m_magnetometer);

    float value;
    if (ua_sensors_magnetic_get_min_value(m_magnetometer, &value) == U_STATUS_SUCCESS)
        m_minValue = value;

    if (ua_sensors_magnetic_get_max_value(m_magnetometer, &value) == U_STATUS_SUCCESS)
        m_maxValue = value;

    if (ua_sensors_magnetic_get_resolution(m_magnetometer, &value) == U_STATUS_SUCCESS)
        m_resolution = value;

    m_available = true;

}

void core::SharedMagnetometer::start()
{
    if (m_available)
        ua_sensors_magnetic_enable(m_magnetometer);
}

void core::SharedMagnetometer::stop()
{
    if (m_available)
        ua_sensors_magnetic_disable(m_magnetometer);
}

qreal core::SharedMagnetometer::getMinDelay() const
{
    return m_minDelay;
}

qreal core::SharedMagnetometer::getMinValue() const
{
    return m_minValue;
}

qreal core::SharedMagnetometer::getMaxValue() const
{
    return m_maxValue;
}

qreal core::SharedMagnetometer::getResolution() const
{
    return m_resolution;
}

void core::SharedMagnetometer::setDelay(quint32 delay_us)
{
    ua_sensors_magnetic_set_event_rate(m_magnetometer, delay_us * 1000);
}

void core::SharedMagnetometer::onMagnetometerReadingCb(UASMagneticEvent *event, void *context)
{
    SharedMagnetometer* ac = static_cast<SharedMagnetometer*>(context);
    if (ac != NULL)
        ac->onMagnetometerReading(event);
}

void core::SharedMagnetometer::onMagnetometerReading(UASMagneticEvent *event)
{
    Q_ASSERT(event != NULL);

    // TODO(tvoss): We should rely on an object pool to recycle Magnetometer reading
    // instances here. We could use a custom deleter for the shared pointer to put
    // instances that have been successfully delivered to slots back into the pool.
    QSharedPointer<QMagnetometerReading> reading(new QMagnetometerReading());

    float value = -1.;

    if (uas_magnetic_event_get_magnetic_field_x(event, &value) == U_STATUS_SUCCESS)
        reading->setX(value);
    if (uas_magnetic_event_get_magnetic_field_y(event, &value) == U_STATUS_SUCCESS)
        reading->setY(value);
    if (uas_magnetic_event_get_magnetic_field_z(event, &value) == U_STATUS_SUCCESS)
        reading->setZ(value);

    reading->setTimestamp(uas_magnetic_event_get_timestamp(event));

    Q_EMIT MagnetometerReadingChanged(reading);
}
