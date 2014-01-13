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
 */

#include <cstdlib>

#include <core/testing/fork_and_run.h>
#include "gtest/gtest.h"

#include <QTemporaryFile>
#include <QtSensors/QAccelerometer>
#include <QtSensors/QOrientationSensor>

using namespace std;

class APITest : public testing::Test
{
  protected:
    virtual void SetUp()
    {
        Q_ASSERT(data_file.open());
        setenv("UBUNTU_PLATFORM_API_SENSOR_TEST", qPrintable(data_file.fileName()), 1);
        setenv("UBUNTU_PLATFORM_API_BACKEND", "libubuntu_application_api_test.so.1", 1);
    }

    virtual void TearDown()
    {
        data_file.remove();
    }

    void set_data(const char* data)
    {
        data_file.write(data);
        data_file.flush();
    }

    QTemporaryFile data_file;
};

TESTP_F(APITest, CreateAccelerator, {
    set_data("create accel 0.5 1000 0.1");

    QAccelerometer sensor;
    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    sensor.setIdentifier("core.accelerometer");
    EXPECT_EQ(sensor.start(), true);
    EXPECT_EQ(sensor.error(), 0);
    EXPECT_EQ(sensor.isConnectedToBackend(), true);
    EXPECT_EQ(sensor.identifier(), "core.accelerometer");

    //EXPECT_EQ(sensor.outputRange(), 0); // FIXME: bug in code
    qoutputrange r = sensor.outputRanges()[0];
    EXPECT_EQ(r.minimum, 0.5f);
    EXPECT_EQ(r.maximum, 1000.0f);
})

TESTP_F(APITest, CreateOrientation, {
    // orientation sensor is based on acceleration
    set_data("create accel -500 500 1");

    QOrientationSensor sensor;
    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    sensor.setIdentifier("core.orientation");
    EXPECT_EQ(sensor.start(), true);
    EXPECT_EQ(sensor.error(), 0);
    EXPECT_EQ(sensor.isConnectedToBackend(), true);
    EXPECT_EQ(sensor.identifier(), "core.orientation");

    //EXPECT_EQ(sensor.outputRange(), 0); // FIXME: bug in code
    qoutputrange r = sensor.outputRanges()[0];
    EXPECT_EQ(r.minimum, -500.0f);
    EXPECT_EQ(r.maximum, 500.0f);
})
