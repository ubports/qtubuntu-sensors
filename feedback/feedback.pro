include(../coverage.pri)
include(../qtubuntu-sensors.pro)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_ubuntu)
PLUGIN_TYPE = feedback

QT = core feedback

HEADERS += core_feedback.h
SOURCES += core_feedback.cpp

target.path += $$[QT_INSTALL_PLUGINS]/feedback
INSTALLS += target

OTHER_FILES += feedback.json

QMAKE_RPATHDIR = $$[QT_INSTALL_LIBS]/qtubuntu-feedback
