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

#ifndef CORE_SHARED_TEMPERATURE_H
#define CORE_SHARED_TEMPERATURE_H

#include <ubuntu/application/sensors/temperature.h>

#include <QAmbientTemperatureReading>
#include <QObject>
#include <QSharedPointer>

namespace core
{
class SharedTemperature : public QObject
{
    Q_OBJECT

public:
    static SharedTemperature& instance();

    void start();
    void stop();

    qreal getMinDelay() const;
    qreal getMinValue() const;
    qreal getMaxValue() const;
    qreal getResolution() const;
    void setDelay(quint32 delay_us);

Q_SIGNALS:
    void temperatureReadingChanged(QSharedPointer<QAmbientTemperatureReading> reading);

private:
    SharedTemperature(QObject *parent = NULL);

    UASensorsTemperature *m_temperature;

    qreal m_minDelay;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_resolution;
    bool m_available;

    // Gets called by the underlying platform when there is a new Temperature reading
    static void onTemperatureReadingCb(UASTemperatureEvent *event, void *context);
    void onTemperatureReading(UASTemperatureEvent *event);
};
}

#endif // CORE_SHARED_TEMPERATURE_H

