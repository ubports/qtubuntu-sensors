#include "core_temperature.h"
#include "core_shared_temperature.h"

#include <QDebug>

core::Temperature::Temperature(QSensor *sensor)
    : QSensorBackend(sensor)
{
    qWarning() << "Temperature instance initializing...";
    // Register the reading instance with the parent
    setReading<QAmbientTemperatureReading>(&m_reading);

    const qreal minDelay = core::SharedTemperature::instance().getMinDelay();
    if (minDelay > -1)
    {
        // Min and max sensor sampling frequencies, in Hz
        addDataRate(minDelay, minDelay * 10);
    }
    addOutputRange(core::SharedTemperature::instance().getMinValue(),
                   core::SharedTemperature::instance().getMaxValue(),
                   core::SharedTemperature::instance().getResolution());

    // Connect to the Temperature's readingChanged signal
    // This has to be a queued connection as the sensor callback
    // from the platform API can happen on an arbitrary thread.
    connect(
        &core::SharedTemperature::instance(),
        SIGNAL(temperatureReadingChanged(QSharedPointer<QAmbientTemperatureReading>)),
        this,
        SLOT(onTemperatureReadingChanged(QSharedPointer<QAmbientTemperatureReading>)),
        Qt::QueuedConnection);

    setDescription(QLatin1String("Temperature Sensor"));
    qWarning() << "Temperature instance initialized.";
}

void core::Temperature::start()
{
    core::SharedTemperature::instance().start();
}

void core::Temperature::stop()
{
    core::SharedTemperature::instance().stop();
}

void core::Temperature::onAmbientTemperatureReadingChanged(QSharedPointer<QAmbientTemperatureReading> reading)
{
    qWarning() << "Temperature instance reading has changed.";
    // Capture the coordinates from the Temperature device
    m_reading.setTemperature(reading->temperature());
    m_reading.setTimestamp(reading->timestamp());

    newReadingAvailable();
}

