/*
 * Copyright (C) 2020 UBports Foundation
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
 *
 * Authored by: Florian Leeber <florian@ubports.com>
 */

#ifndef CORE_COMPASS_H
#define CORE_COMPASS_H

#include <QObject>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QAccelerometer>
#include <QMagnetometer>
#include <QGyroscope>
#include <qsensorbackend.h>

#define FILTER_COEFFICIENT 0.75f
#define EPSILON 0.000000001f
#define NS2S (1.0f / 1000000000.0f)
#define PI 3.1415926

namespace core
{
class Compass : public QSensorBackend
{

    Q_OBJECT
public:
    static char const * const id;
    Compass(QSensor *sensor);
    ~Compass();
    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

signals:
    void sensorError(int);

private:
    QAccelerometer *_gravitySensor;
    QMagnetometer *_magnetmeter;
    QGyroscope *_gyroscope;
    mutable QCompassReading _compassReading;
    bool _gyroscopeEnabled;

    float _gravity[3];
    float _gyro[3];
    float _orientation[3]; //alias: accMagOrientation
    float _geomagnetic[3];
    float *_gyroMatrix;
    float _gyroOrientation[3];
    float _fusedOrientation[3];
    float _timestamp;
    bool _initState;

    void checkValues();
    void calculateFusedOrientation();
    float *matrixMultiplication(float *A, float *B);
    float *getRotationMatrixFromOrientation(float *o);
    void gyroFunction(QGyroscopeReading *event);
    void getRotationVectorFromGyro(float *gyroValues, float *deltaRotationVector, float timeFactor);
    static void getRotationMatrixFromVector(float *R, size_t lenR, float *rotationVector, size_t lenRotationVector);
    static bool getRotationMatrix(float *R, size_t lenR, float *I, size_t lenI, float *gravity, float *geomagnetic);
    static float *getOrientation(float *R, size_t lenR, float *values);

private slots:
    void onAccelerometerChanged();
    void onMagnetometerChanged();
    void onGyroscopeChanged();
};

}

#endif // CORE_COMPASS_H
