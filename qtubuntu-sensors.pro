include(coverage.pri)

TEMPLATE = subdirs

SUBDIRS += \
    feedback \
    lib \
    plugin

plugin.depends = lib
import.depends = lib
