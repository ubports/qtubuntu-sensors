include(../coverage.pri)
TEMPLATE = lib
TARGET = qtubuntu-sensors

QT = core sensors

HEADERS += orientationsensor.h \
    orientationsensor_p.h \
    accelerometersensor.h \
    accelerometersensor_p.h

SOURCES += orientationsensor.cpp \
    accelerometersensor.cpp

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target
