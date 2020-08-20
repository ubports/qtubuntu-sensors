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

#include "core_shared_pressure.h"

#include <QDebug>
#include <QMetaType>
#include <qmath.h>

Q_DECLARE_METATYPE(QSharedPointer<QPressureReading>)

core::SharedPressure& core::SharedPressure::instance()
{
    static core::SharedPressure instance;
    return instance;
}

core::SharedPressure::SharedPressure(QObject *parent)
    : QObject(parent),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0),
      m_available(false)
{
    qWarning() << "SharedPressure instance initializing...";
    qRegisterMetaType<QSharedPointer<QPressureReading> >("QSharedPointer<QPressureReading>");
    m_pressure = ua_sensors_pressure_new();
    if (m_pressure == NULL)
        return;

    ua_sensors_pressure_set_reading_cb(
                m_pressure,
                core::SharedPressure::onPressureReadingCb,
                static_cast<void *>(this));

    // Get the minimum sensor reading delay
    m_minDelay = ua_sensors_pressure_get_min_delay(m_pressure);

    float value;
    if (ua_sensors_pressure_get_min_value(m_pressure, &value) == U_STATUS_SUCCESS)
        m_minValue = value;

    if (ua_sensors_pressure_get_max_value(m_pressure, &value) == U_STATUS_SUCCESS)
        m_maxValue = value;

    if (ua_sensors_pressure_get_resolution(m_pressure, &value) == U_STATUS_SUCCESS)
        m_resolution = value;

    m_available = true;
    qWarning() << "SharedPressure instance initialized.";

}

void core::SharedPressure::start()
{
    if (m_available)
        ua_sensors_pressure_enable(m_pressure);
}

void core::SharedPressure::stop()
{
    if (m_available)
        ua_sensors_pressure_disable(m_pressure);
}

qreal core::SharedPressure::getMinDelay() const
{
    return m_minDelay;
}

qreal core::SharedPressure::getMinValue() const
{
    return m_minValue;
}

qreal core::SharedPressure::getMaxValue() const
{
    return m_maxValue;
}

qreal core::SharedPressure::getResolution() const
{
    return m_resolution;
}

void core::SharedPressure::setDelay(quint32 delay_us)
{
    ua_sensors_pressure_set_event_rate(m_pressure, delay_us * 1000);
}

void core::SharedPressure::onPressureReadingCb(UASPressureEvent *event, void *context)
{
    SharedPressure* ac = static_cast<SharedPressure*>(context);
    if (ac != NULL)
        ac->onPressureReading(event);
}

void core::SharedPressure::onPressureReading(UASPressureEvent *event)
{
    qWarning() << "SharedPressure instance new reading.";
    Q_ASSERT(event != NULL);

    // TODO(tvoss): We should rely on an object pool to recycle Pressure reading
    // instances here. We could use a custom deleter for the shared pointer to put
    // instances that have been successfully delivered to slots back into the pool.
    QSharedPointer<QPressureReading> reading(new QPressureReading());

    float value = -1.;

    if (uas_pressure_event_get_pressure(event, &value) == U_STATUS_SUCCESS)
        reading->setPressure(value);

    reading->setTimestamp(uas_pressure_event_get_timestamp(event));

    Q_EMIT pressureReadingChanged(reading);
}

