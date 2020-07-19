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

#ifndef CORE_SHARED_GYROSCOPE_H
#define CORE_SHARED_GYROSCOPE_H

#include <ubuntu/application/sensors/gyroscope.h>

#include <QGyroscopeReading>
#include <QObject>
#include <QSharedPointer>

namespace core
{
class SharedGyroscope : public QObject
{
    Q_OBJECT

public:
    static SharedGyroscope& instance();

    void start();
    void stop();

    qreal getMinDelay() const;
    qreal getMinValue() const;
    qreal getMaxValue() const;
    qreal getResolution() const;
    void setDelay(quint32 delay_us);

Q_SIGNALS:
    void gyroscopeReadingChanged(QSharedPointer<QGyroscopeReading> reading);

private:
    SharedGyroscope(QObject *parent = NULL);

    UASensorsGyroscope *m_gyroscope;

    qreal m_minDelay;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_resolution;
    bool m_available;

    // Gets called by the underlying platform when there is a new Gyroscope reading
    static void onGyroscopeReadingCb(UASGyroscopeEvent *event, void *context);
    void onGyroscopeReading(UASGyroscopeEvent *event);
};
}

#endif // CORE_SHARED_GYROSCOPE_H
