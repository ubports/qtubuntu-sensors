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

#include "core_compass.h"
#include "core_shared_magnetometer.h"
#include "core_shared_accelerometer.h"

#include <QDebug>
#include <QtCore/qmath.h>

#define RADIANS_TO_DEGREES 57.2957795
#define DEGREES_TO_RADIANS 0.017453292
#define GRAVITY_EARTH 9.80665f
#define FILTER_FACTOR 0.24f
#define LIST_COUNT 10

core::Compass::Compass(QSensor *sensor)
    : QSensorBackend(sensor)
{
    // Register the reading instance with the parent
    setReading<QCompassReading>(&m_reading);

    const qreal minDelay = core::SharedMagnetometer::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedMagnetometer::instance().getMinValue(),
                   core::SharedMagnetometer::instance().getMaxValue(),
                   core::SharedMagnetometer::instance().getResolution());

    // Connect to the Compass's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedMagnetometer::instance(),
        SIGNAL(MagnetometerReadingChanged(QSharedPointer<QMagnetometerReading>)),
        this, 
        SLOT(onMagnetometerReadingChanged(QSharedPointer<QMagnetometerReading>)),
        Qt::QueuedConnection);

    connect(
        &core::SharedAccelerometer::instance(),
        SIGNAL(accelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)),
        this, 
        SLOT(onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Compass Sensor"));
}

void core::Compass::start()
{
    core::SharedMagnetometer::instance().start();
    core::SharedAccelerometer::instance().start();
}

void core::Compass::stop()
{
    core::SharedMagnetometer::instance().stop();
    core::SharedAccelerometer::instance().stop();
}

void core::Compass::onMagnetometerReadingChanged(QSharedPointer<QMagnetometerReading> reading)
{

    magX = reading->y();
    magY = reading->x();
    magZ = reading->z();
    level = (int)reading->calibrationLevel();

    magX = oldMagX + FILTER_FACTOR * (magX - oldMagX);
    magY = oldMagY + FILTER_FACTOR * (magY - oldMagY);
    magZ = oldMagZ + FILTER_FACTOR * (magZ - oldMagZ);
    oldMagX = magX;
    oldMagY = magY;
    oldMagZ = magZ;

}

void core::Compass::onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading)
{

    qreal Gx = reading->y();
    qreal Gy = reading->x();
    qreal Gz = -reading->z();

    qreal divisor = qSqrt(Gx * Gx + Gy * Gy + Gz * Gz);
    qreal normalizedGx = Gx / divisor;
    qreal normalizedGy = Gy / divisor;
    qreal normalizedGz = Gz / divisor;

    ///////////////
    /// this algorithm is from Circuit Cellar Aug 2012
    ///  by Mark Pedley
    /// Electronic Compass: Tilt Compensation & Calibration
    /// There are no restrictions on your use of the software listed in the
    /// Circuit Cellar magazine.
    /// http://circuitcellar.com/
    ///
    qreal Psi = 0;
    qreal The = 0;
    qreal Phi = 0;
    qreal sinAngle = 0;
    qreal cosAngle = 0;
    qreal fBfx = 0;
    qreal fBfy = 0;

    /* calculate roll angle Phi (-180deg, 180deg) and sin, cos */
    Phi = qAtan2(normalizedGy, normalizedGz); /* Equation 2 */
    sinAngle = qSin(Phi);
    cosAngle = qCos(Phi);

    /* de-rotate magY roll angle Phi */
    fBfy = magY * cosAngle - magZ * sinAngle; /* Equation 5 y component */
    magZ = magY * sinAngle + magZ * cosAngle;
    normalizedGz = normalizedGy * sinAngle + normalizedGz * cosAngle;

    /* calculate pitch angle Theta (-90deg, 90deg) and sin, cos*/
    The = qAtan(-normalizedGx / normalizedGz);  /* Equation 3 */
    sinAngle = qSin(The);
    cosAngle = qCos(The);

    /* de-rotate magY pitch angle Theta */
    fBfx = magX * cosAngle + magZ * sinAngle; /* Equation 5 x component */

    /* calculate yaw = ecompass angle psi (-180deg, 180deg) */
    Psi = (qAtan2(-fBfy, fBfx) * RADIANS_TO_DEGREES); /* Equation 7 */

    qreal heading = Psi * FILTER_FACTOR + oldHeading * (1.0 - FILTER_FACTOR);


    m_reading.setAzimuth((int)(heading + 360) % 360);
    m_reading.setCalibrationLevel(level);
    m_reading.setTimestamp(reading->timestamp());

    oldHeading = heading;

    newReadingAvailable();
}

