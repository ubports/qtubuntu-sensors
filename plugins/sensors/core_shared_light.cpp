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

#include "core_shared_light.h"

#include <QDebug>
#include <QMetaType>
#include <qmath.h>

Q_DECLARE_METATYPE(QSharedPointer<QLightReading>)

core::SharedLight& core::SharedLight::instance()
{
    static core::SharedLight instance;
    return instance;
}

core::SharedLight::SharedLight(QObject *parent)
    : QObject(parent),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0),
      m_available(false)
{
    qRegisterMetaType<QSharedPointer<QLightReading> >("QSharedPointer<QLightReading>");
    m_light = ua_sensors_light_new();
    if (m_light == NULL)
        return;

    ua_sensors_light_set_reading_cb(
                m_light,
                core::SharedLight::onLightReadingCb,
                static_cast<void *>(this));

    // Get the minimum sensor reading delay
    m_minDelay = ua_sensors_light_get_min_delay(m_light);

    float value;
    if (ua_sensors_light_get_min_value(m_light, &value) == U_STATUS_SUCCESS)
        m_minValue = value;

    if (ua_sensors_light_get_max_value(m_light, &value) == U_STATUS_SUCCESS)
        m_maxValue = value;

    if (ua_sensors_light_get_resolution(m_light, &value) == U_STATUS_SUCCESS)
        m_resolution = value;

    m_available = true;

}

void core::SharedLight::start()
{
    if (m_available)
        ua_sensors_light_enable(m_light);
}

void core::SharedLight::stop()
{
    if (m_available)
        ua_sensors_light_disable(m_light);
}

qreal core::SharedLight::getMinDelay() const
{
    return m_minDelay;
}

qreal core::SharedLight::getMinValue() const
{
    return m_minValue;
}

qreal core::SharedLight::getMaxValue() const
{
    return m_maxValue;
}

qreal core::SharedLight::getResolution() const
{
    return m_resolution;
}

void core::SharedLight::setDelay(quint32 delay_us)
{
    ua_sensors_light_set_event_rate(m_light, delay_us * 1000);
}

void core::SharedLight::onLightReadingCb(UASLightEvent *event, void *context)
{
    SharedLight* ac = static_cast<SharedLight*>(context);
    if (ac != NULL)
        ac->onLightReading(event);
}

void core::SharedLight::onLightReading(UASLightEvent *event)
{
    Q_ASSERT(event != NULL);

    // TODO(tvoss): We should rely on an object pool to recycle Light reading
    // instances here. We could use a custom deleter for the shared pointer to put
    // instances that have been successfully delivered to slots back into the pool.
    QSharedPointer<QLightReading> reading(new QLightReading());

    float value = -1.;

    if (uas_light_event_get_light(event, &value) == U_STATUS_SUCCESS)
        reading->setLux(value);

    reading->setTimestamp(uas_light_event_get_timestamp(event));

    Q_EMIT lightReadingChanged(reading);
}

