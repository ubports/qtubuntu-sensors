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

#ifndef CORE_COMPASS_H
#define CORE_COMPASS_H

#include <QCompassReading>
#include <QMagnetometerReading>
#include <QAccelerometerReading>

#include <qsensorbackend.h>

namespace core
{
class Compass : public QSensorBackend
{
    Q_OBJECT
public:
    inline static const char* id()
    {
        return "core.compass";
    }

    Compass(QSensor *sensor);
    virtual ~Compass() = default;

    void start();
    void stop();

public Q_SLOTS:
    void onMagnetometerReadingChanged(QSharedPointer<QMagnetometerReading> reading);
    void onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading);

private:
    QCompassReading m_reading;

    qreal magX;
    qreal magY;
    qreal magZ;
    qreal oldMagX;
    qreal oldMagY;
    qreal oldMagZ;
    int level;
    qreal oldHeading;
};
}

#endif // CORE_COMPASS_H
