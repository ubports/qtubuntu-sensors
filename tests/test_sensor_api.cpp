/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Martin Pitt <martin.pitti@ubuntu.com>
 *
 * This test suite uses the simulated sensor test backend from
 * libubuntu-application-api-test1. Please see
 * /usr/share/doc/libubuntu-application-api-test1/README.md for how to use it
 * and the format of the sensor data files.
 */

#include <cstdlib>
#include <chrono>
#include <queue>
#include <iostream>

#include <core/testing/fork_and_run.h>
#include <gtest/gtest.h>

#include <QTemporaryFile>
#include <QThread>
#include <QCoreApplication>
#include <QtSensors/QAccelerometer>
#include <QtSensors/QOrientationSensor>

using namespace std;

// structures for logging received events from signals
struct AccelEvent {
    qreal x, y, z;
    chrono::time_point<chrono::system_clock> time;
};

struct OrientationEvent {
    QOrientationReading::Orientation orientation;
    chrono::time_point<chrono::system_clock> time;
};

/*******************************************
 *
 * Tests with real sensor backend
 *
 *******************************************/

class RealBackendTest : public testing::Test
{
    virtual void SetUp()
    {
    }
};

TESTP_F(RealBackendTest, CreateAccelerometer, {
    QAccelerometer sensor;
    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    sensor.setIdentifier("core.accelerometer");

    // this can succeed for fail depending on whether the hardware we run this
    // on actually exists; but it should never crash
    if (sensor.start()) {
        EXPECT_EQ(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), true);
        EXPECT_EQ(sensor.accelerationMode(), QAccelerometer::Combined);
    } else {
        EXPECT_NE(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), false);
    }

    EXPECT_EQ(sensor.identifier(), "core.accelerometer");
})

TESTP_F(RealBackendTest, CreateOrientation, {
    QOrientationSensor sensor;
    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    sensor.setIdentifier("core.orientation");

    // this can succeed for fail depending on whether the hardware we run this
    // on actually exists; but it should never crash
    if (sensor.start()) {
        EXPECT_EQ(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), true);
    } else {
        EXPECT_NE(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), false);
    }

    EXPECT_EQ(sensor.identifier(), "core.orientation");
})

/*******************************************
 *
 * Tests with default sensor backend
 *
 *******************************************/

class DefaultBackendTest : public testing::Test
{
    virtual void SetUp()
    {
    }
};

TESTP_F(DefaultBackendTest, CreateAccelerometer, {
    QAccelerometer sensor;
    // don't set any particular identifier here

    // this can succeed for fail depending on whether the hardware we run this
    // on actually exists; but it should never crash
    if (sensor.start()) {
        EXPECT_EQ(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), true);
    } else {
        EXPECT_NE(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), false);
    }

    cout << "default backend connected to " << sensor.identifier().constData() << endl;
})

TESTP_F(DefaultBackendTest, CreateOrientation, {
    QOrientationSensor sensor;
    // don't set any particular identifier here

    // this can succeed for fail depending on whether the hardware we run this
    // on actually exists; but it should never crash
    if (sensor.start()) {
        EXPECT_EQ(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), true);
    } else {
        EXPECT_NE(sensor.error(), 0);
        EXPECT_EQ(sensor.isConnectedToBackend(), false);
    }

    cout << "default backend connected to " << sensor.identifier().constData() << endl;
})

