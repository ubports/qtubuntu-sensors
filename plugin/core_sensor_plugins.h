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

#ifndef CORE_SENSOR_PLUGINS_H
#define CORE_SENSOR_PLUGINS_H

#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QAccelerometer>
#include <QOrientationSensor>

#include <QDebug>

namespace core
{
class SensorPlugins
        : public QObject,
          public QSensorPluginInterface,
          public QSensorBackendFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.qt-project.Qt.QSensorPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(QSensorPluginInterface)

public:
    // From QSensorPluginInterface
    void registerSensors();

    // From QSensorBackendFactory
    QSensorBackend *createBackend(QSensor *sensor);
};
}

#endif // CORE_SENSOR_PLUGINS_H
