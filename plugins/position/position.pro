QT += location core

TEMPLATE = lib
CONFIG += link_pkgconfig plugin
TARGET = $$qtLibraryTarget(qtposition_ubuntu)
PLUGIN_TYPE = position

QT = location core

HEADERS += \
    core_geo_position_info_source.h \
    core_geo_position_info_source_factory.h

SOURCES += \
    core_geo_position_info_source.cpp \
    core_geo_position_info_source_factory.cpp

INCLUDEPATH += $$PWD/

PKGCONFIG += ubuntu-platform-api
LIBS += -lubuntu_application_api

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += plugin.json
