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

#include "core_orientation_sensor.h"

#include "core_shared_accelerometer.h"

#include <math.h>

#include <QDebug>

const float core::OrientationSensor::m_accelDelta = 7.35;

#define CLOCKWISE           1
#define COUNTER_CLOCKWISE   0

#define RAD_TO_DEG(x)   (x * 57.2957)

// Values taken from Android's orientation helper
const float core::OrientationSensor::m_minAccel = 5.8;
const float core::OrientationSensor::m_maxAccel = 13.8;
const int core::OrientationSensor::m_maxTilt = 75;

const int core::OrientationSensor::m_tiltTolerance[4][2] = {
    { -25, 70 },
    { -25, 65 },
    { -25, 60 },
    { -25, 65 }
};

core::OrientationSensor::OrientationSensor(QSensor *sensor)
    : QSensorBackend(sensor), m_lastX(0), m_lastY(0), m_lastZ(0)
{
    // Register the reading instance with the parent
    setReading<QOrientationReading>(&m_reading);

    const qreal minDelay = core::SharedAccelerometer::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedAccelerometer::instance().getMinValue(),
                   core::SharedAccelerometer::instance().getMaxValue(),
                   core::SharedAccelerometer::instance().getResolution());

    // Connect to the accelerometer's readingChanged signal
    connect(&core::SharedAccelerometer::instance(),
            SIGNAL(accelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)), 
            this, 
            SLOT(onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading>)),
            Qt::QueuedConnection);

    setDescription(QLatin1String("Orientation Sensor"));
}

void core::OrientationSensor::start()
{
    core::SharedAccelerometer::instance().start();
    core::SharedAccelerometer::instance().setDelay(66667);
}

void core::OrientationSensor::stop()
{
    core::SharedAccelerometer::instance().stop();
}

int nearestRotation = 0;

void core::OrientationSensor::onAccelerometerReadingChanged(QSharedPointer<QAccelerometerReading> reading)
{
    float x = reading->x();
    float y = reading->y();
    float z = reading->z();

    /*
     * Low pass filter to remove jitter.
     * Code inspired by Android's orientation helper
     */  
    unsigned long now = reading->timestamp();
    unsigned long then = m_lastFilter;

    double timeDelta = (now-then)/1000000;
    if (timeDelta < 66) return; // If events come faster than 15Hz, drop

    float alpha = timeDelta / (200 + timeDelta);
    x = alpha * (x - m_lastX) + m_lastX;
    y = alpha * (y - m_lastY) + m_lastY;
    z = alpha * (z - m_lastZ) + m_lastZ;

    m_lastX = x;
    m_lastY = y;
    m_lastZ = z;
    m_lastFilter = now;

    int orientation = round(RAD_TO_DEG((atan2(-x,y))));

    if (orientation < 0)
        orientation += 360;

    if (m_lastOrientation == orientation) // static hit, not considering
        return;

    m_lastOrientation = orientation;
    
    float magnitude = sqrt(x*x + y*y + z*z);

    /*
     * External acceleration and tilt decisions
     * Some code inspired by Android's orientation helper
     */
    if (magnitude < m_minAccel || magnitude > m_maxAccel)
        return;

    int tiltAngle = round(RAD_TO_DEG(asin(z / magnitude)));

    if (tiltAngle > m_maxTilt)
        return;
      
    if (m_readingCache.orientation() == QOrientationReading::Undefined)
    {
        m_readingCache.setOrientation(QOrientationReading::TopUp);
        nearestRotation = 0;
    }
    else
    {
        nearestRotation = (orientation + 45) / 90;
        
        if (nearestRotation > 3)
            nearestRotation = 0;

        if (!(tiltAngle >= m_tiltTolerance[nearestRotation][0] && tiltAngle <= m_tiltTolerance[nearestRotation][1]))
            return;

        /*
         * Calculate rotation-hint offsets
         * Inspired by Android code
         */
        if (m_lastRotation == nearestRotation
                || nearestRotation == (m_lastRotation + 1) % 4) {
            int lowerBound = (nearestRotation*90) - 45 + 45/2;
            if (nearestRotation == 0) {
                if (orientation >= 315 && orientation < lowerBound + 360) {
                    return;
                }
            }
            else
            {
                if (orientation < lowerBound) {
                    return;
                }
            }
        }

        if (m_lastRotation == nearestRotation
                || nearestRotation == (m_lastRotation + 3) % 4) {
            int upperBound = (nearestRotation*90) + 45 - 45/2;
            if (nearestRotation == 0) {
                if (orientation <= 45 && orientation > upperBound) {
                    return;
                }
            }
            else
            {
                if (orientation > upperBound) {
                    return;                    
                }
            }
        }
    }
   
    
    switch (nearestRotation)
    {
    case 0: m_reading.setOrientation(QOrientationReading::TopUp); break;
    case 1: m_reading.setOrientation(QOrientationReading::LeftUp); break;
    case 2: m_reading.setOrientation(QOrientationReading::TopDown); break;
    case 3: m_reading.setOrientation(QOrientationReading::RightUp); break;
    }
    
    if (m_reading.orientation() != m_readingCache.orientation())
    {
        newReadingAvailable();
        m_readingCache.setOrientation(m_reading.orientation());
    }

    m_lastRotation = nearestRotation;
}
