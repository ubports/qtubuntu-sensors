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

#ifndef CORE_FEEDBACK_H
#define CORE_FEEDBACK_H

#include <QtCore/QList>
#include <QtCore/QObject>

#include <qfeedbackplugininterfaces.h>

#include <ubuntu/application/sensors/haptic.h>

namespace core
{
class Feedback : public QObject, public QFeedbackHapticsInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtFeedbackPlugin" FILE "feedback.json")

    Q_INTERFACES(QFeedbackHapticsInterface)

public:
    Feedback();
    virtual ~Feedback();

    // From QFeedbackHapticsInterface
    virtual PluginPriority pluginPriority();

    QList<QFeedbackActuator*> actuators();

    QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *);
    void setEffectState (const QFeedbackHapticsEffect* effect, QFeedbackEffect::State state);

    bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability);
    void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);

    void updateEffectProperty(const QFeedbackHapticsEffect* effect, EffectProperty property);

private:
    QList<QFeedbackActuator*> actuatorList;

    void vibrateOnce(const QFeedbackEffect* effect);

    UASensorsHaptic* m_vibrator;
};
}

#endif // CORE_FEEDBACK_H
