QT += location core

TEMPLATE = lib
CONFIG += link_pkgconfig plugin
TARGET = $$qtLibraryTarget(qtposition_ubuntu)
PLUGIN_TYPE = position

QT = location core

HEADERS += \
    qgeopositioninfosource_ubuntu_p.h \
    qgeopositioninfosourcefactory_ubuntu.h

SOURCES += \
    qgeopositioninfosource_ubuntu.cpp \
    qgeopositioninfosourcefactory_ubuntu.cpp

INCLUDEPATH += $$PWD/

PKGCONFIG += ubuntu-platform-api
LIBS += -lubuntu_application_api

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += plugin.json
