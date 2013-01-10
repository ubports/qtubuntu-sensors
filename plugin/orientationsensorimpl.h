/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Jim Hodapp <jim.hodapp@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ORIENTATIONSENSORIMPL_H
#define ORIENTATIONSENSORIMPL_H

#include "common.h"

#include <qsensorbackend.h>
#include <QOrientationReading>

class OrientationSensorImpl : public QSensorBackend
{
    Q_OBJECT

public:
    static char const * const id;

    OrientationSensorImpl(QSensor *sensor = NULL);
    virtual ~OrientationSensorImpl();

    void start();
    void stop();

Q_SIGNALS:
    void readingChanged();

public Q_SLOTS:
    void onAccelerometerReadingChanged();

private:
    // The distance from the center, right or left, that will trigger an
    // orientation change when the user rotates the target device.
    static const float m_accelDelta;

    AccelerometerCommon *m_accelCommon;
    QOrientationReading m_reading;
};

#endif
