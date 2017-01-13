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
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

core::Feedback::Feedback() : QObject(),
                             enabled(false),
                             state(QFeedbackEffect::Stopped)
{
    actuatorList << createFeedbackActuator(this, 42);
}

core::Feedback::~Feedback()
{
}

QFeedbackInterface::PluginPriority core::Feedback::pluginPriority()
{
    return PluginHighPriority;
}

QList<QFeedbackActuator*> core::Feedback::actuators()
{
    return actuatorList;
}

void core::Feedback::setActuatorProperty(const QFeedbackActuator&, ActuatorProperty prop, const QVariant &value)
{
    switch (prop)
    {
    case Enabled:
        enabled = value.toBool();
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
    case Enabled: result = enabled; break;
    }

    return result;
}

bool core::Feedback::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    bool result = false;

    switch(cap)
    {
    case QFeedbackActuator::Envelope: result = true; break;
    case QFeedbackActuator::Period: result = false; break;
    }

    return result;
}

void core::Feedback::updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty)
{
}

void core::Feedback::hapticsVibrateReply(QDBusPendingCallWatcher *watcher, int period, int repeat)
{
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        qWarning() << "Failed to vibrate with pattern:" << reply.error().message();
        state = QFeedbackEffect::Stopped;
    } else {
        if ((repeat == QFeedbackEffect::Infinite) || (--repeat > 0))
            QTimer::singleShot(period, [=]() { vibrate(period, repeat); });
        else
            state = QFeedbackEffect::Stopped;
    }

    watcher->deleteLater();
}

void core::Feedback::vibrate(int period, int repeat)
{
    if (!(period && repeat))
        state = QFeedbackEffect::Stopped;

    if (state != QFeedbackEffect::Running) {
        // Maybe stopped/paused before this async call.
        return;
    }


    QDBusInterface iface("com.canonical.usensord",
                         "/com/canonical/usensord/haptic",
                         "com.canonical.usensord.haptic");

    QDBusPendingCall call = iface.asyncCall("Vibrate", (uint)period);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [=](){ hapticsVibrateReply(watcher, period, repeat); });
}

void core::Feedback::startVibration(const QFeedbackHapticsEffect *effect)
{
    int duration = effect->duration();
    if (duration == 0)
        duration = 150;

    int period = effect->period();
    int repeat;
    if ((duration == QFeedbackEffect::Infinite) || (duration < 0)) {
        // If duration is set to QFeedbackEffect::Infinite or a negative
        // value, we repeat this effect forever until stopped. The
        // effective period should have been set to a positive value or
        // 150ms by default.
        duration = QFeedbackEffect::Infinite;
        repeat = QFeedbackEffect::Infinite;
        if (period <= 0)
            period = 150;
    } else if (period <= 0) {
        // If duration is set to a positive value and period is invalid,
        // then use duration as period.
        repeat = 1;
        period = duration;
    } else {
        // Otherwise, repeat this effect as many times as the duration
        // may cover the effect period.
        repeat = (duration + period - 1) / period;
    }

    vibrate(period, repeat);
}

void core::Feedback::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
{
    this->state = state;
    switch (state)
    {
    case QFeedbackEffect::Stopped:
        break;
    case QFeedbackEffect::Paused:
        break;
    case QFeedbackEffect::Running:
        QTimer::singleShot(0, [=]() { startVibration(effect); });
        break;
    case QFeedbackEffect::Loading:
        break;
    }
}

QFeedbackEffect::State core::Feedback::effectState(const QFeedbackHapticsEffect *)
{
    return state;
}
