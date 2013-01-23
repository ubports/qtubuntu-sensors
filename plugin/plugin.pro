include(../coverage.pri)
include(../aalsensor-plugins.pro)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_aal)
PLUGIN_TYPE = sensors

QT = sensors core

message("UBUNTU_APP_API_INCLUDEPATH =" $$UBUNTU_APP_API_INCLUDEPATH)
INCLUDEPATH += $$PWD/../lib \
    $$UBUNTU_APP_API_INCLUDEPATH

LIBS += -L$$OUT_PWD/../lib -laalsensors
LIBS += -L$$UBUNTU_APP_API_LIBPATH -lubuntu_application_api

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
