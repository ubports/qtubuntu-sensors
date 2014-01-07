/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeopositioninfosource_ubuntu_p.h"

#include <QtCore>

#include <ubuntu/application/location/service.h>
#include <ubuntu/application/location/session.h>

#include <ubuntu/application/location/heading_update.h>
#include <ubuntu/application/location/position_update.h>
#include <ubuntu/application/location/velocity_update.h>

QT_BEGIN_NAMESPACE

struct QGeoPositionInfoSourceUbuntu::Private
{
    static const unsigned int empty_creation_flags = 0;
    
    static void processPositionUpdate(
        UALocationPositionUpdate* position,
        void* context)
    {
        if (!position)
            return;

        Private* thiz = static_cast<Private*>(context);

        if (!thiz)
            return;

        thiz->handlePositionUpdate(position);
    }

    static void processHeadingUpdate(
        UALocationHeadingUpdate* heading,
        void* context)
    {
        if (!heading)
            return;

        Private* thiz = static_cast<Private*>(context);

        if (!thiz)
            return;

        thiz->handleHeadingUpdate(heading);
    }

    static void processVelocityUpdate(
        UALocationVelocityUpdate* velocity,
        void* context)
    {
        if (!velocity)
            return;

        Private* thiz = static_cast<Private*>(context);

        if (!thiz)
            return;

        thiz->handleVelocityUpdate(velocity);
    }

    void handlePositionUpdate(UALocationPositionUpdate* position)
    {
        QGeoCoordinate coord(
            ua_location_position_update_get_latitude_in_degree(position),
            ua_location_position_update_get_longitude_in_degree(position),
            ua_location_position_update_has_altitude(position) ? ua_location_position_update_get_altitude_in_meter(position) : 0);
        
        lastKnownPosition.setCoordinate(coord);
        lastKnownPosition.setTimestamp(
            QDateTime::fromMSecsSinceEpoch(
                ua_location_position_update_get_timestamp(position)/1000));

        QMetaObject::invokeMethod(
            parent,
            "positionUpdated",
            Qt::QueuedConnection,
            Q_ARG(QGeoPositionInfo, lastKnownPosition));        
    }

    void handleHeadingUpdate(UALocationHeadingUpdate* heading)
    {
        lastKnownPosition.setAttribute(
            QGeoPositionInfo::Direction,
            ua_location_heading_update_get_heading_in_degree(heading));

        lastKnownPosition.setTimestamp(
            QDateTime::fromMSecsSinceEpoch(
                ua_location_heading_update_get_timestamp(heading)/1000));

        QMetaObject::invokeMethod(
            parent,
            "positionUpdated",
            Qt::QueuedConnection,
            Q_ARG(QGeoPositionInfo, lastKnownPosition));
    };

    void handleVelocityUpdate(UALocationVelocityUpdate* velocity)
    {
        lastKnownPosition.setAttribute(
            QGeoPositionInfo::GroundSpeed,
            ua_location_velocity_update_get_velocity_in_meters_per_second(velocity));

        lastKnownPosition.setTimestamp(
            QDateTime::fromMSecsSinceEpoch(
                ua_location_velocity_update_get_timestamp(velocity)/1000));

        QMetaObject::invokeMethod(
            parent,
            "positionUpdated",
            Qt::QueuedConnection,
            Q_ARG(QGeoPositionInfo, lastKnownPosition));
    };

    Private(QGeoPositionInfoSourceUbuntu* parent)
            : parent(parent),
              session(ua_location_service_create_session_for_high_accuracy(empty_creation_flags))
    {
        qRegisterMetaType<QGeoPositionInfo>("QGeoPositionInfo");

        ua_location_service_session_set_position_updates_handler(
            session,
            processPositionUpdate,
            this);

        ua_location_service_session_set_heading_updates_handler(
            session,
            processHeadingUpdate,
            this);

        ua_location_service_session_set_velocity_updates_handler(
            session,
            processVelocityUpdate,
            this);
    }

    ~Private()
    {
        ua_location_service_session_unref(session);
    }

    QGeoPositionInfoSourceUbuntu* parent;
    UALocationServiceSession* session;
    QGeoPositionInfo lastKnownPosition;
    QTimer timer;
};

QGeoPositionInfoSourceUbuntu::QGeoPositionInfoSourceUbuntu(QObject *parent)
        : QGeoPositionInfoSource(parent), d(new Private(this))
{
    d->timer.setSingleShot(true);
    QObject::connect(&d->timer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
}

QGeoPositionInfoSourceUbuntu::~QGeoPositionInfoSourceUbuntu()
{
}

void QGeoPositionInfoSourceUbuntu::setUpdateInterval(int msec)
{
    (void) msec;
}

void QGeoPositionInfoSourceUbuntu::setPreferredPositioningMethods(PositioningMethods methods)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
}

QGeoPositionInfo QGeoPositionInfoSourceUbuntu::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    Q_UNUSED(fromSatellitePositioningMethodsOnly);
    return d->lastKnownPosition;
}

QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceUbuntu::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

void QGeoPositionInfoSourceUbuntu::startUpdates()
{
    ua_location_service_session_start_position_updates(d->session);
    ua_location_service_session_start_heading_updates(d->session);
    ua_location_service_session_start_velocity_updates(d->session);
}

int QGeoPositionInfoSourceUbuntu::minimumUpdateInterval() const {
    return 500;
}

void QGeoPositionInfoSourceUbuntu::stopUpdates()
{
    ua_location_service_session_stop_position_updates(d->session);
    ua_location_service_session_stop_heading_updates(d->session);
    ua_location_service_session_stop_velocity_updates(d->session);
}

void QGeoPositionInfoSourceUbuntu::requestUpdate(int timeout)
{
    if (d->timer.isActive())
    {
        return;
    }

    startUpdates();
    d->timer.start(timeout);
}

QGeoPositionInfoSource::Error QGeoPositionInfoSourceUbuntu::error() const
{
    return UnknownSourceError;
}

QT_END_NAMESPACE
