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
#include <iostream>

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

struct OrientationEvent {
    QOrientationReading::Orientation orientation;
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

        // ensure the queues are clear
        while (accel_events.size() > 0)
            accel_events.pop();
        while (orientation_events.size() > 0)
            orientation_events.pop();
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

    // check the next event in orientation_events for expected values and delay
    void check_orientation_event(QOrientationReading::Orientation value, unsigned ms)
    {
        auto e = orientation_events.front();
        orientation_events.pop();
        EXPECT_EQ(e.orientation, value);
        // allow -5 to +10 ms slack in event time
        auto delay = chrono::duration_cast<chrono::milliseconds>(e.time - start_time).count();
        EXPECT_GE(delay, ms - 10);
        EXPECT_LE(delay, ms + 20);
    }

    QTemporaryFile data_file;
    QAccelerometer accel_sensor;
    QOrientationSensor orientation_sensor;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    queue<struct AccelEvent> accel_events;
    queue<struct OrientationEvent> orientation_events;
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
    accel_sensor.setIdentifier("core.accelerometer");

    QObject::connect(&accel_sensor, &QAccelerometer::readingChanged, [=]() { 
        auto r = accel_sensor.reading();
        accel_events.push({r->x(), r->y(), r->z(), chrono::system_clock::now()});
    });

    EXPECT_EQ(accel_sensor.start(), true);

    start_time = chrono::system_clock::now();
    run_events(550);

    EXPECT_EQ(accel_events.size(), 3);

    check_accel_event(0.0, -9.9, 0.0, 10);
    check_accel_event(1.5, 400, 0.5, 210);
    check_accel_event(-1.0, -9.8, -0.5, 510);
})

TESTP_F(APITest, AcceleratorReadings, {
    set_data("create accel -500 500 0.1\n"
             "10 accel 0 -9.9 0\n"
             "200 accel 1.5 400 0.5\n"
             "300 accel -1 -9.8 -0.5\n");

    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    accel_sensor.setIdentifier("core.accelerometer");

    EXPECT_EQ(accel_sensor.start(), true);

    // initial value
    auto reading = accel_sensor.reading();
    EXPECT_FLOAT_EQ(reading->x(), 0.0);
    EXPECT_FLOAT_EQ(reading->y(), 0.0);
    EXPECT_FLOAT_EQ(reading->z(), 0.0);

    run_events(50);
    reading = accel_sensor.reading();
    EXPECT_FLOAT_EQ(reading->x(), 0.0);
    EXPECT_FLOAT_EQ(reading->y(), -9.9);
    EXPECT_FLOAT_EQ(reading->z(), 0.0);

    run_events(200);
    reading = accel_sensor.reading();
    EXPECT_FLOAT_EQ(reading->x(), 1.5);
    EXPECT_FLOAT_EQ(reading->y(), 400);
    EXPECT_FLOAT_EQ(reading->z(), 0.5);

    run_events(350);
    reading = accel_sensor.reading();
    EXPECT_FLOAT_EQ(reading->x(), -1);
    EXPECT_FLOAT_EQ(reading->y(), -9.8);
    EXPECT_FLOAT_EQ(reading->z(), -0.5);
})

TESTP_F(APITest, OrientationEvents, {
        /* test some "parallel to coordinate axes" conditions, as well as some
         * ~ 45 degrees angles; we want a hysteresis there, i. e. it should not
         * flip back and forth when wiggling around the diagonals but only when
         * it's mostly pointing towards an axis
         * coordinate system:
         * http://qt-project.org/doc/qt-5.1/qtsensors/qaccelerometerreading.html
         */
    set_data("create accel -500 500 0.1\n"
             "10 accel 0 9.8 0\n"  // TopUp
             "100 accel 6.9 6.9 0\n"  // turning left
             "150 accel 8.1 2.3 0\n"  // almost turned left, should trigger RightUp
             "50 accel -7.1 6.9 0\n"  // turn right, wiggle around diagonal several times
             "30 accel -6.5 7.1 0\n" 
             "30 accel -7.0 6.0 0\n"
             "30 accel -7.0 6.0 0\n"
             "30 accel -8.0 3.0 0\n"  // finally turn right enough to trigger LeftUp
             "30 accel 0 -9.8 0\n"    // TopDown
             "30 accel 0 0 9.8\n"    // FaceUp
             "30 accel 0 0 -9.8\n"    // FaceDown
             );

    orientation_sensor.setIdentifier("core.orientation");

    QObject::connect(&orientation_sensor, &QOrientationSensor::readingChanged, [=]() { 
        auto r = orientation_sensor.reading();
        orientation_events.push({r->orientation(), chrono::system_clock::now()});
    });

    EXPECT_EQ(orientation_sensor.start(), true);

    start_time = chrono::system_clock::now();
    run_events(550);  // must be long enough to catch all events

    EXPECT_EQ(orientation_events.size(), 6);

    check_orientation_event(QOrientationReading::TopUp, 10);
    check_orientation_event(QOrientationReading::RightUp, 260);
    check_orientation_event(QOrientationReading::LeftUp, 430);
    check_orientation_event(QOrientationReading::TopDown, 460);
    check_orientation_event(QOrientationReading::FaceUp, 490);
    check_orientation_event(QOrientationReading::FaceDown, 520);
})

TESTP_F(APITest, OrientationReading, {
    set_data("create accel -500 500 0.1\n"
             "10 accel 0 9.8 0\n"  // TopUp
             "20 accel 6.9 6.9 0\n"  // turning left
             "20 accel 8.1 2.3 0\n"  // almost turned left, should trigger RightUp
             );

    orientation_sensor.setIdentifier("core.orientation");

    EXPECT_EQ(orientation_sensor.start(), true);

    // initial value
    EXPECT_EQ(orientation_sensor.reading()->orientation(), QOrientationReading::Undefined);

    // TopUp after first event
    run_events(20);
    EXPECT_EQ(orientation_sensor.reading()->orientation(), QOrientationReading::TopUp);

    // not changed for "turning left" yet
    run_events(20);
    EXPECT_EQ(orientation_sensor.reading()->orientation(), QOrientationReading::TopUp);

    // but changed after "almost turned left"
    run_events(20);
    EXPECT_EQ(orientation_sensor.reading()->orientation(), QOrientationReading::RightUp);
})
