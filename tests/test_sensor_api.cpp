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
#include <chrono>
#include <queue>

#include <core/testing/fork_and_run.h>
#include "gtest/gtest.h"

#include <QTemporaryFile>
#include <QThread>
#include <QCoreApplication>
#include <QtSensors/QAccelerometer>
#include <QtSensors/QOrientationSensor>

using namespace std;

struct AccelEvent {
   qreal x, y, z;
   std::chrono::time_point<std::chrono::system_clock> time;
};


class APITest : public testing::Test
{
  protected:
    virtual void SetUp()
    {
        Q_ASSERT(data_file.open());
        setenv("UBUNTU_PLATFORM_API_SENSOR_TEST", qPrintable(data_file.fileName()), 1);
        setenv("UBUNTU_PLATFORM_API_BACKEND", "libubuntu_application_api_test.so.1", 1);

        // ensure the queue is clear
        while (accel_events.size() > 0)
            accel_events.pop();
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

    // run main loop for given number of ms
    void run_events(unsigned ms)
    {
        int argc = 0;
        char* argv[] = { NULL };
        const unsigned step = 10; // ms per iteration
        unsigned count = ms / step;
        QCoreApplication app(argc, argv);

        while (count-- > 0) {
            QThread::msleep(step);
            app.processEvents();
        }
    }

    // check the next event in accel_events for expected values and delay
    void check_accel_event(qreal x, qreal y, qreal z, unsigned ms)
    {
        auto e = accel_events.front();
        accel_events.pop();
        EXPECT_FLOAT_EQ(e.x, x);
        EXPECT_FLOAT_EQ(e.y, y);
        EXPECT_FLOAT_EQ(e.z, z);
        // allow -5 to +10 ms slack in event time
        auto delay = chrono::duration_cast<chrono::milliseconds>(e.time - start_time).count();
        EXPECT_GE(delay, ms - 10);
        EXPECT_LE(delay, ms + 20);
    }

    QTemporaryFile data_file;
    QAccelerometer accel;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    queue<struct AccelEvent> accel_events;
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
    EXPECT_EQ(sensor.accelerationMode(), QAccelerometer::Combined);

    //EXPECT_EQ(sensor.outputRange(), 0); // FIXME: bug in code
    qoutputrange r = sensor.outputRanges()[0];
    EXPECT_FLOAT_EQ(r.minimum, 0.5f);
    EXPECT_FLOAT_EQ(r.maximum, 1000.0f);

    // defined initial values
    auto reading = sensor.reading();
    EXPECT_FLOAT_EQ(reading->x(), 0.0);
    EXPECT_FLOAT_EQ(reading->y(), 0.0);
    EXPECT_FLOAT_EQ(reading->z(), 0.0);
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

    // defined initial value
    EXPECT_EQ(sensor.reading()->orientation(), QOrientationReading::Undefined);
})

TESTP_F(APITest, AcceleratorEvents, {
    set_data("create accel -500 500 0.1\n"
             "10 accel 0 -9.9 0\n"
             "200 accel 1.5 400 0.5\n"
             "300 accel -1 -9.8 -0.5\n");

    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    accel.setIdentifier("core.accelerometer");

    QObject::connect(&accel, &QAccelerometer::readingChanged, [=]() { 
        auto r = accel.reading();
        accel_events.push({r->x(), r->y(), r->z(), chrono::system_clock::now()});
    });

    EXPECT_EQ(accel.start(), true);

    start_time = chrono::system_clock::now();
    run_events(550);

    EXPECT_EQ(accel_events.size(), 3);

    check_accel_event(0.0, -9.9, 0.0, 10);
    check_accel_event(1.5, 400, 0.5, 210);
    check_accel_event(-1.0, -9.8, -0.5, 510);
})
