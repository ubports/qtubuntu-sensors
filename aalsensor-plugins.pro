include(coverage.pri)

TEMPLATE = subdirs

isEmpty(UBUNTU_APP_API_INCLUDEPATH) {
  UBUNTU_APP_API_INCLUDEPATH = "/usr/include/hybris"
}
message("UBUNTU_APP_API_INCLUDEPATH =" $$UBUNTU_APP_API_INCLUDEPATH)

isEmpty(UBUNTU_APP_API_LIBPATH) {
  UBUNTU_APP_API_LIBPATH = "/usr/lib/arm-linux-gnueabihf"
}
message("UBUNTU_APP_API_LIBPATH =" $$UBUNTU_APP_API_LIBPATH)

SUBDIRS += \
    lib \
    plugin

plugin.depends = lib
import.depends = lib
