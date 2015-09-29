/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include "core_feedback.h"

#include <qfeedbackactuator.h>

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

core::Feedback::Feedback() : QObject(),
                             m_vibrator(NULL)
{
    actuatorList << createFeedbackActuator(this, 42);

    if (qgetenv("UBUNTU_PLATFORM_API_BACKEND").isNull())
        return;

    m_vibrator = ua_sensors_haptic_new();
    ua_sensors_haptic_enable(m_vibrator);
}

core::Feedback::~Feedback()
{
    if (m_vibrator) {
        ua_sensors_haptic_destroy(m_vibrator);
    }
}

QFeedbackInterface::PluginPriority core::Feedback::pluginPriority()
{
    return PluginHighPriority;
}

QList<QFeedbackActuator*> core::Feedback::actuators()
{
    return actuatorList;
}

void core::Feedback::setActuatorProperty(const QFeedbackActuator&, ActuatorProperty prop, const QVariant &)
{
    switch (prop)
    {
    case Enabled:
        break;
    default:
        break;
    }
}

QVariant core::Feedback::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    QVariant result;

    switch (prop)
    {
    case Name: result = QString::fromLocal8Bit("Ubuntu Vibrator"); break;
    case State: result = actuator.isValid() ? QFeedbackActuator::Ready : QFeedbackActuator::Unknown; break;
    case Enabled: result = true; break;
    }

    return result;
}

bool core::Feedback::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    bool result = false;

    switch(cap)
    {
    case QFeedbackActuator::Envelope:
    case QFeedbackActuator::Period: result = true; break;
    }

    return result;
}

void core::Feedback::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty)
{
    if (effect->period() != -1) {
        /* Not currently supported */
        reportError(effect, QFeedbackEffect::UnknownError);
    }
}

void core::Feedback::vibrateOnce(const QFeedbackEffect* effect)
{
    int effectiveDuration = effect->duration();
    switch (effectiveDuration)
    {
        case QFeedbackEffect::Infinite:
        case 0:
            effectiveDuration = 150;
    }

    if (m_vibrator)
        ua_sensors_haptic_vibrate_once(m_vibrator, effectiveDuration);
}

void core::Feedback::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
{
    switch (state)
    {
    case QFeedbackEffect::Stopped:
        break;
    case QFeedbackEffect::Paused:
        break;
    case QFeedbackEffect::Running:
        vibrateOnce(effect);
        break;
    case QFeedbackEffect::Loading:
        break;
    }
}

QFeedbackEffect::State core::Feedback::effectState(const QFeedbackHapticsEffect *)
{
    // We don't currently support on-going vibrations
    // This can be added when moving to a vibration service
    return QFeedbackEffect::Stopped;
}
