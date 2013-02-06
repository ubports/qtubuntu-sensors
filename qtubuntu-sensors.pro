include(coverage.pri)

TEMPLATE = subdirs

SUBDIRS += \
    lib \
    plugin

plugin.depends = lib
import.depends = lib
