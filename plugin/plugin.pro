include(../coverage.pri)
TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_aal)
PLUGIN_TYPE = sensors

QT = sensors core

INCLUDEPATH += $$PWD/../lib \
    /usr/include/hybris
LIBS += -L$$OUT_PWD/../lib -laalsensors -lhybris_ics -lubuntu_application_api

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
