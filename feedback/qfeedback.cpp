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

#include <qfeedbackactuator.h>
#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

QFeedbackMir::QFeedbackMir() : QObject(qApp),
    QFeedbackThemeInterface()
{
    const int nbDev = 1;
    for (int i = 0; i < nbDev; ++i) {
        actuatorList << createFeedbackActuator(this, i);
    }
}

QFeedbackMir::~QFeedbackMir()
{
}

QFeedbackInterface::PluginPriority QFeedbackMir::pluginPriority()
{
    return PluginNormalPriority;
}

QList<QFeedbackActuator*> QFeedbackMir::actuators()
{
    return actuatorList;
}

void QFeedbackMir::setActuatorProperty(const QFeedbackActuator &, ActuatorProperty prop, const QVariant &)
{
    switch (prop)
    {
    case Enabled:
        break;
    default:
        break;
    }
}

QVariant QFeedbackMir::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    switch (prop)
    {
    case Name:
        {
            return QString::fromLocal8Bit("Command line vibrator");
        }

    case State:
        {
            QFeedbackActuator::State ret = QFeedbackActuator::Unknown;
            if (actuator.isValid()) {
                ret = QFeedbackActuator::Ready;
            }

            return ret;
        }
    case Enabled:
        {
            // no global vibration policy, always enabled
            return true;
        }
    default:
        return QVariant();
    }
}

bool QFeedbackMir::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    switch(cap)
    {
    case QFeedbackActuator::Envelope:
    case QFeedbackActuator::Period:
        return true;
    default:
        return false;
    }
}

void QFeedbackMir::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty)
{
    if (effect->period() > 0) {
        /* Not currently supported */
        reportError(effect, QFeedbackEffect::UnknownError);
    } else {
        /* one-off */
    }

}

void QFeedbackMir::vibrateOnce(const QFeedbackEffect* effect)
{
    int effectiveDuration = effect->duration();
    switch (effectiveDuration)
    {
        case QFeedbackEffect::Infinite:
        case 0:
            effectiveDuration = 150;
    }

    QString ifaceFilename("/sys/class/timed_output/vibrator/enable");
    if (!QFileInfo(ifaceFilename).exists())
    {
        qWarning() << ifaceFilename << "not available on this system";
        return;
    }

    // This is fire and forget, no support for on-going vibrations or errors
    // Later this will be replaced with using a vibration service
    QProcess vibrator;
    vibrator.setStandardOutputFile(ifaceFilename);
    // The process launches async
    vibrator.start("echo", QStringList() << QString("%1").arg(effectiveDuration));
}

void QFeedbackMir::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
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
    default:
        break;
    }
}

QFeedbackEffect::State QFeedbackMir::effectState(const QFeedbackHapticsEffect *)
{
    // We don't currently support on-going vibrations
    // This can be added when moving to a vibration service
    return QFeedbackEffect::Stopped;
}

void QFeedbackMir::setLoaded(QFeedbackFileEffect *effect, bool)
{
    // Not currently supported
     reportError(effect, QFeedbackEffect::UnknownError);
}

void QFeedbackMir::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    switch (state)
    {
    case QFeedbackEffect::Stopped:
        break;
    case QFeedbackEffect::Paused:
        break;
    case QFeedbackEffect::Running:
        break;
    default:
        reportError(effect, QFeedbackEffect::UnknownError);
        break;
    }
}

QFeedbackEffect::State QFeedbackMir::effectState(const QFeedbackFileEffect *)
{
    return QFeedbackEffect::Stopped;
}

int QFeedbackMir::effectDuration(const QFeedbackFileEffect *effect)
{
    return 0;
}

QStringList QFeedbackMir::supportedMimeTypes()
{
    return QStringList() << QLatin1String("vibra/ivt");
}

bool QFeedbackMir::play(QFeedbackEffect::Effect)
{
    QFeedbackHapticsEffect themeEffect;
    vibrateOnce(&themeEffect);
    return true;
}

