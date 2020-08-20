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

#include "core_accelerometer.h"
#include "core_gyroscope.h"
#include "core_magnetometer.h"
#include "core_orientation_sensor.h"
#include "core_pressure.h"
#include "core_temperature.h"
#include "core_light.h"
#include "core_sensor_plugins.h"

#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QAccelerometer>
#include <QOrientationSensor>

void core::SensorPlugins::registerSensors()
{
    QSensorManager::registerBackend(
                QOrientationSensor::type,
                core::OrientationSensor::id(),
                this);

    QSensorManager::registerBackend(
                QAccelerometer::type,
                core::Accelerometer::id(),
                this);

    QSensorManager::registerBackend(
                QGyroscope::type,
                core::Gyroscope::id(),
                this);

    QSensorManager::registerBackend(
                QMagnetometer::type,
                core::Magnetometer::id(),
                this);

    QSensorManager::registerBackend(
                QAmbientTemperatureSensor::type,
                core::Temperature::id(),
                this);

    QSensorManager::registerBackend(
                QLightSensor::type,
                core::Light::id(),
                this);

    QSensorManager::registerBackend(
                QPressureSensor::type,
                core::Pressure::id(),
                this);
}

// Instantiate all sensor backends here:
QSensorBackend* core::SensorPlugins::createBackend(QSensor *sensor)
{
    if (qgetenv("UBUNTU_PLATFORM_API_BACKEND").isNull())
        return NULL;

    if (sensor->identifier() == core::OrientationSensor::id())
        return new core::OrientationSensor(sensor);

    if (sensor->identifier() == core::Accelerometer::id())
        return new core::Accelerometer(sensor);

    if (sensor->identifier() == core::Gyroscope::id())
        return new core::Gyroscope(sensor);

    if (sensor->identifier() == core::Magnetometer::id())
        return new core::Magnetometer(sensor);

    if (sensor->identifier() == core::Pressure::id())
        return new core::Pressure(sensor);

    if (sensor->identifier() == core::Light::id())
        return new core::Light(sensor);

    if (sensor->identifier() == core::Temperature::id())
        return new core::Temperature(sensor);

    return NULL;
}
