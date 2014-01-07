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
 * Author: Thomas Voß <thomas.voss@canonical.com>
 */

#include "core_geo_position_info_source.h"

#include <QtCore>

#include <ubuntu/application/location/service.h>
#include <ubuntu/application/location/session.h>

#include <ubuntu/application/location/heading_update.h>
#include <ubuntu/application/location/position_update.h>
#include <ubuntu/application/location/velocity_update.h>

struct core::GeoPositionInfoSource::Private
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

    Private(core::GeoPositionInfoSource* parent)
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

    core::GeoPositionInfoSource* parent;
    UALocationServiceSession* session;
    QGeoPositionInfo lastKnownPosition;
    QTimer timer;
};

core::GeoPositionInfoSource::GeoPositionInfoSource(QObject *parent)
        : QGeoPositionInfoSource(parent), d(new Private(this))
{
    d->timer.setSingleShot(true);
    QObject::connect(&d->timer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
}

core::GeoPositionInfoSource::~GeoPositionInfoSource()
{
}

void core::GeoPositionInfoSource::setUpdateInterval(int msec)
{
    (void) msec;
}

void core::GeoPositionInfoSource::setPreferredPositioningMethods(PositioningMethods methods)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
}

QGeoPositionInfo core::GeoPositionInfoSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    Q_UNUSED(fromSatellitePositioningMethodsOnly);
    return d->lastKnownPosition;
}

QGeoPositionInfoSource::PositioningMethods core::GeoPositionInfoSource::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

void core::GeoPositionInfoSource::startUpdates()
{
    ua_location_service_session_start_position_updates(d->session);
    ua_location_service_session_start_heading_updates(d->session);
    ua_location_service_session_start_velocity_updates(d->session);
}

int core::GeoPositionInfoSource::minimumUpdateInterval() const {
    return 500;
}

void core::GeoPositionInfoSource::stopUpdates()
{
    ua_location_service_session_stop_position_updates(d->session);
    ua_location_service_session_stop_heading_updates(d->session);
    ua_location_service_session_stop_velocity_updates(d->session);
}

void core::GeoPositionInfoSource::requestUpdate(int timeout)
{
    if (d->timer.isActive())
    {
        return;
    }

    startUpdates();
    d->timer.start(timeout);
}

QGeoPositionInfoSource::Error core::GeoPositionInfoSource::error() const
{
    return UnknownSourceError;
}
