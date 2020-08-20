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

#include "core_shared_temperature.h"

#include <QDebug>
#include <QMetaType>
#include <qmath.h>

Q_DECLARE_METATYPE(QSharedPointer<QAmbientTemperatureReading>)

core::SharedTemperature& core::SharedTemperature::instance()
{
    static core::SharedTemperature instance;
    return instance;
}

core::SharedTemperature::SharedTemperature(QObject *parent)
    : QObject(parent),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0),
      m_available(false)
{
    qWarning() << "SharedTemperature instance initializing...";
    qRegisterMetaType<QSharedPointer<QAmbientTemperatureReading> >("QSharedPointer<QAmbientTemperatureReading>");
    m_temperature = ua_sensors_temperature_new();
    if (m_temperature == NULL)
        return;

    ua_sensors_temperature_set_reading_cb(
                m_temperature,
                core::SharedTemperature::onTemperatureReadingCb,
                static_cast<void *>(this));

    // Get the minimum sensor reading delay
    m_minDelay = ua_sensors_temperature_get_min_delay(m_temperature);

    float value;
    if (ua_sensors_temperature_get_min_value(m_temperature, &value) == U_STATUS_SUCCESS)
        m_minValue = value;

    if (ua_sensors_temperature_get_max_value(m_temperature, &value) == U_STATUS_SUCCESS)
        m_maxValue = value;

    if (ua_sensors_temperature_get_resolution(m_temperature, &value) == U_STATUS_SUCCESS)
        m_resolution = value;

    m_available = true;
    qWarning() << "SharedTemperature instance initialized.";

}

void core::SharedTemperature::start()
{
    if (m_available)
        ua_sensors_temperature_enable(m_temperature);
}

void core::SharedTemperature::stop()
{
    if (m_available)
        ua_sensors_temperature_disable(m_temperature);
}

qreal core::SharedTemperature::getMinDelay() const
{
    return m_minDelay;
}

qreal core::SharedTemperature::getMinValue() const
{
    return m_minValue;
}

qreal core::SharedTemperature::getMaxValue() const
{
    return m_maxValue;
}

qreal core::SharedTemperature::getResolution() const
{
    return m_resolution;
}

void core::SharedTemperature::setDelay(quint32 delay_us)
{
    ua_sensors_temperature_set_event_rate(m_temperature, delay_us * 1000);
}

void core::SharedTemperature::onTemperatureReadingCb(UASTemperatureEvent *event, void *context)
{
    SharedTemperature* ac = static_cast<SharedTemperature*>(context);
    if (ac != NULL)
        ac->onTemperatureReading(event);
}

void core::SharedTemperature::onTemperatureReading(UASTemperatureEvent *event)
{
    qWarning() << "SharedTemperature instance new reading.";
    Q_ASSERT(event != NULL);

    // TODO(tvoss): We should rely on an object pool to recycle Temperature reading
    // instances here. We could use a custom deleter for the shared pointer to put
    // instances that have been successfully delivered to slots back into the pool.
    QSharedPointer<QAmbientTemperatureReading> reading(new QAmbientTemperatureReading());

    float value = -1.;

    if (uas_temperature_event_get_temperature(event, &value) == U_STATUS_SUCCESS)
        reading->setTemperature(value);

    reading->setTimestamp(uas_temperature_event_get_timestamp(event));

    Q_EMIT temperatureReadingChanged(reading);
}


