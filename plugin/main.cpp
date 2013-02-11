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

#include "orientationsensor.h"
#include "orientationsensorimpl.h"
#include "accelerometersensor.h"
#include "accelerometersensorimpl.h"

#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QAccelerometer>
#include <QOrientationSensor>

#include <QDebug>

class AalSensorPlugins : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.Qt.QSensorPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(QSensorPluginInterface)

public:
    void registerSensors()
    {
        qDebug() << "Registered the AalSensorPlugin types" << endl;
        QSensorManager::registerBackend(QOrientationSensor::type, OrientationSensorImpl::id, this);
        QSensorManager::registerBackend(QAccelerometer::type, AccelerometerSensorImpl::id, this);
    }

    // Instantiate all sensor backends here:
    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == OrientationSensorImpl::id)
            return new OrientationSensorImpl(sensor);

        if (sensor->identifier() == AccelerometerSensorImpl::id)
            return new AccelerometerSensorImpl(sensor);

        return NULL;
    }
};

#include "main.moc"
