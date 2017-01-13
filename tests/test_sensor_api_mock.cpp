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
 * Tests with simulated sensor backend
 *
 *******************************************/

class SimBackendTest : public testing::Test
{
  public:
    SimBackendTest()
    {
        QCoreApplication::setLibraryPaths(QStringList("../plugins/"));
	
        accel_sensor = new QAccelerometer();
        orientation_sensor = new QOrientationSensor();
    }

  protected:
    virtual void SetUp()
    {
	bool openResult = data_file.open();
        EXPECT_EQ(openResult, true);
        setenv("UBUNTU_PLATFORM_API_SENSOR_TEST", qPrintable(data_file.fileName()), 1);
        setenv("UBUNTU_PLATFORM_API_BACKEND", "test", 1);

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

    // check the next event in orientation_events for expected value and delay
    void check_orientation_event(QOrientationReading::Orientation value, unsigned ms)
    {
        auto e = orientation_events.front();
        orientation_events.pop();
        EXPECT_EQ(e.orientation, value);
        // allow -5 to +10 ms slack in event time
        auto delay = chrono::duration_cast<chrono::milliseconds>(e.time - start_time).count();
        EXPECT_GE(delay, ms - 10);
        EXPECT_LE(delay, ms + 40);
    }

    QTemporaryFile data_file;
    QAccelerometer *accel_sensor;
    QOrientationSensor *orientation_sensor;
    chrono::time_point<chrono::system_clock> start_time;
    queue<struct AccelEvent> accel_events;
    queue<struct OrientationEvent> orientation_events;
};

TESTP_F(SimBackendTest, CreateAccelerometer, {
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

TESTP_F(SimBackendTest, CreateOrientation, {
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

TESTP_F(SimBackendTest, AccelerometerEvents, {
    set_data("create accel -500 500 0.1\n"
             "10 accel 0 -9.9 0\n"
             "200 accel 1.5 400 0.5\n"
             "300 accel -1 -9.8 -0.5\n");

    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    accel_sensor->setIdentifier("core.accelerometer");
    
    QObject::connect(accel_sensor, &QAccelerometer::readingChanged, [=]() { 
        auto r = accel_sensor->reading();
        accel_events.push({r->x(), r->y(), r->z(), chrono::system_clock::now()});
    });

    EXPECT_EQ(accel_sensor->start(), true);

    start_time = chrono::system_clock::now();
    run_events(550);

    EXPECT_EQ(accel_events.size(), 3u);

    check_accel_event(0.0, -9.9, 0.0, 10);
    check_accel_event(1.5, 400, 0.5, 210);
    check_accel_event(-1.0, -9.8, -0.5, 510);
})

TESTP_F(SimBackendTest, AccelerometerReadings, {
    set_data("create accel -500 500 0.1\n"
             "10 accel 0 -9.9 0\n"
             "200 accel 1.5 400 0.5\n"
             "300 accel -1 -9.8 -0.5\n");

    // connect to the qtubuntu-sensors backend; default is dummy, and there
    // does not seem to be a way to use data/Sensors.conf
    accel_sensor->setIdentifier("core.accelerometer");

    EXPECT_EQ(accel_sensor->start(), true);

    // initial value
    auto reading = accel_sensor->reading();
    EXPECT_FLOAT_EQ(reading->x(), 0.0);
    EXPECT_FLOAT_EQ(reading->y(), 0.0);
    EXPECT_FLOAT_EQ(reading->z(), 0.0);

    run_events(50);
    reading = accel_sensor->reading();
    EXPECT_FLOAT_EQ(reading->x(), 0.0);
    EXPECT_FLOAT_EQ(reading->y(), -9.9);
    EXPECT_FLOAT_EQ(reading->z(), 0.0);

    run_events(200);
    reading = accel_sensor->reading();
    EXPECT_FLOAT_EQ(reading->x(), 1.5);
    EXPECT_FLOAT_EQ(reading->y(), 400);
    EXPECT_FLOAT_EQ(reading->z(), 0.5);

    run_events(350);
    reading = accel_sensor->reading();
    EXPECT_FLOAT_EQ(reading->x(), -1);
    EXPECT_FLOAT_EQ(reading->y(), -9.8);
    EXPECT_FLOAT_EQ(reading->z(), -0.5);
})

TESTP_F(SimBackendTest, OrientationReading, {
    set_data("create accel -500 500 0.001\n"
             "70 accel 0.347 9.768 0.162\n"
             "70 accel -0.600 9.947 0.174\n"
             "70 accel -0.885 11.061 -0.177\n"
             "70 accel -2.121 10.397 -0.468\n"
             "70 accel -4.242 8.664 -0.023\n"
             "70 accel -5.882 7.633 0.169\n"
             "70 accel -7.031 5.822 -0.268\n"
             "70 accel -8.138 3.775 0.564\n"
             "70 accel -9.616 1.854 1.981\n"
             "70 accel -9.562 -0.195 1.138\n"
             "70 accel -10.373 -0.597 1.802\n"
             );


    orientation_sensor->setIdentifier("core.orientation");

    EXPECT_EQ(orientation_sensor->start(), true);

    // initial value
    EXPECT_EQ(orientation_sensor->reading()->orientation(), QOrientationReading::Undefined);

    // Right up after stream of svents
    run_events(1400);
    EXPECT_EQ(orientation_sensor->reading()->orientation(), QOrientationReading::LeftUp);
})
