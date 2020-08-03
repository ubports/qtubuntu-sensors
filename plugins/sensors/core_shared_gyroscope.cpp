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

#include "core_shared_gyroscope.h"

#include <QDebug>
#include <QMetaType>
#include <qmath.h>

Q_DECLARE_METATYPE(QSharedPointer<QGyroscopeReading>)

core::SharedGyroscope& core::SharedGyroscope::instance()
{
    static core::SharedGyroscope instance;
    return instance;
}

core::SharedGyroscope::SharedGyroscope(QObject *parent)
    : QObject(parent),
      m_minDelay(-1),
      m_minValue(0.0),
      m_maxValue(0.0),
      m_resolution(0.0),
      m_available(false)
{
    qWarning() << "SharedGyroscope instance initializing...";
    qRegisterMetaType<QSharedPointer<QGyroscopeReading> >("QSharedPointer<QGyroscopeReading>");
    m_gyroscope = ua_sensors_gyroscope_new();
    if (m_gyroscope == NULL)
        return;

    ua_sensors_gyroscope_set_reading_cb(
                m_gyroscope,
                core::SharedGyroscope::onGyroscopeReadingCb,
                static_cast<void *>(this));

    // Get the minimum sensor reading delay
    m_minDelay = ua_sensors_gyroscope_get_min_delay(m_gyroscope);

    float value;
    if (ua_sensors_gyroscope_get_min_value(m_gyroscope, &value) == U_STATUS_SUCCESS)
        m_minValue = value;

    if (ua_sensors_gyroscope_get_max_value(m_gyroscope, &value) == U_STATUS_SUCCESS)
        m_maxValue = value;

    if (ua_sensors_gyroscope_get_resolution(m_gyroscope, &value) == U_STATUS_SUCCESS)
        m_resolution = value;

    m_available = true;
    qWarning() << "SharedGyroscope instance initialized.";

}

void core::SharedGyroscope::start()
{
    if (m_available)
        ua_sensors_gyroscope_enable(m_gyroscope);
}

void core::SharedGyroscope::stop()
{
    if (m_available)
        ua_sensors_gyroscope_disable(m_gyroscope);
}

qreal core::SharedGyroscope::getMinDelay() const
{
    return m_minDelay;
}

qreal core::SharedGyroscope::getMinValue() const
{
    return m_minValue;
}

qreal core::SharedGyroscope::getMaxValue() const
{
    return m_maxValue;
}

qreal core::SharedGyroscope::getResolution() const
{
    return m_resolution;
}

void core::SharedGyroscope::setDelay(quint32 delay_us)
{
    ua_sensors_gyroscope_set_event_rate(m_gyroscope, delay_us * 1000);
}

void core::SharedGyroscope::onGyroscopeReadingCb(UASGyroscopeEvent *event, void *context)
{
    SharedGyroscope* ac = static_cast<SharedGyroscope*>(context);
    if (ac != NULL)
        ac->onGyroscopeReading(event);
}

void core::SharedGyroscope::onGyroscopeReading(UASGyroscopeEvent *event)
{
    qWarning() << "SharedGyroscope instance new reading.";
    Q_ASSERT(event != NULL);

    // TODO(tvoss): We should rely on an object pool to recycle Gyroscope reading
    // instances here. We could use a custom deleter for the shared pointer to put
    // instances that have been successfully delivered to slots back into the pool.
    QSharedPointer<QGyroscopeReading> reading(new QGyroscopeReading());

    float value = -1.;

    if (uas_gyroscope_event_get_rate_of_rotation_around_x(event, &value) == U_STATUS_SUCCESS)
        reading->setX(qRadiansToDegrees(value));
    if (uas_gyroscope_event_get_rate_of_rotation_around_y(event, &value) == U_STATUS_SUCCESS)
        reading->setY(qRadiansToDegrees(value));
    if (uas_gyroscope_event_get_rate_of_rotation_around_z(event, &value) == U_STATUS_SUCCESS)
        reading->setZ(qRadiansToDegrees(value));

    reading->setTimestamp(uas_gyroscope_event_get_timestamp(event));

    Q_EMIT gyroscopeReadingChanged(reading);
}
