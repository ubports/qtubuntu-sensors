include(../coverage.pri)
include(../qtubuntu-sensors.pro)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtubuntu_sensors_plugins)
PLUGIN_TYPE = sensors

QT = sensors core

INCLUDEPATH += $$PWD/

LIBS += -L$$OUT_PWD/
LIBS += -lubuntu_application_api

HEADERS += \
    core_accelerometer.h \
    core_orientation_sensor.h \
    core_sensor_plugins.h \
    core_shared_accelerometer.h

SOURCES += \
    core_accelerometer.cpp \
    core_orientation_sensor.cpp \
    core_sensor_plugins.cpp \
    core_shared_accelerometer.cpp

target.path += $$[QT_INSTALL_PLUGINS]/sensors
INSTALLS += target

OTHER_FILES += plugin.json

QMAKE_RPATHDIR = $$[QT_INSTALL_LIBS]/qtubuntu-sensors
