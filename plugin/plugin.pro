include(../coverage.pri)
include(../qtubuntu-sensors.pro)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtubuntu_sensors_plugins)
PLUGIN_TYPE = sensors

QT = sensors core

INCLUDEPATH += $$PWD/../lib

LIBS += -L$$OUT_PWD/../lib -lqtubuntu_sensors
LIBS += -lubuntu_application_api

HEADERS += \
    common.h \
    orientationsensorimpl.h \
    accelerometersensorimpl.h

SOURCES += \
    common.cpp \
    orientationsensorimpl.cpp \
    accelerometersensorimpl.cpp \
    main.cpp

target.path += $$[QT_INSTALL_PLUGINS]/sensors
INSTALLS += target

OTHER_FILES += plugin.json
