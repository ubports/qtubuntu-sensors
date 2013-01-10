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

#ifndef ACCELEROMETERIMPL_H
#define ACCELEROMETERIMPL_H

#include "common.h"

#include <qsensorbackend.h>

class AccelerometerSensorImpl : public QSensorBackend
{
    Q_OBJECT

public:
    static char const * const id;

    AccelerometerSensorImpl(QSensor *sensor = NULL);
    virtual ~AccelerometerSensorImpl();

    void start();
    void stop();

Q_SIGNALS:
    void readingChanged();

public Q_SLOTS:
    void onAccelerometerReadingChanged();

private:
    AccelerometerCommon *m_accelCommon;
    QAccelerometerReading m_reading;
};

#endif
