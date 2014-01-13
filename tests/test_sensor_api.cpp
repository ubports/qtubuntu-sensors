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
#include <cstdio>
#include <queue>

#include <core/testing/fork_and_run.h>
#include "gtest/gtest.h"

#include <QtSensors/QAccelerometer>

using namespace std;

class APITest : public testing::Test
{
  protected:
    virtual void SetUp()
    {
        snprintf(data_file, sizeof(data_file), "%s", "/tmp/sensor_test.XXXXXX");
        data_fd = mkstemp(data_file);
        if (data_fd < 0) {
            perror("mkstemp");
            abort();
        }
        setenv("UBUNTU_PLATFORM_API_SENSOR_TEST", data_file, 1);
        setenv("UBUNTU_PLATFORM_API_BACKEND", "libubuntu_application_api_test.so.1", 1);
    }

    virtual void TearDown()
    {
        unlink(data_file);
    }

    void set_data(const char* data)
    {
        Q_ASSERT(write(data_fd, data, strlen(data)) > 0);
        fsync(data_fd);
    }

    char data_file[100];
    int data_fd;
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
