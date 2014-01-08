find_package(PkgConfig)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sensors REQUIRED)

pkg_check_modules(UBUNTU_PLATFORM_API ubuntu-platform-api REQUIRED)

qt5_add_resources(SENSORS_RESOURCES sensors.qrc)

add_library(
  qtubuntu_sensors_plugins SHARED

  core_accelerometer.h
  core_orientation_sensor.h
  core_sensor_plugins.h
  core_shared_accelerometer.h

  core_accelerometer.cpp
  core_orientation_sensor.cpp
  core_sensor_plugins.cpp
  core_shared_accelerometer.cpp

  ${SENSORS_RESOURCES}
)

# Ideally, we would read the plugin installation location from cmake
# but this does not work currently.
set(PLUGIN_INSTALL_LOCATION "${CMAKE_INSTALL_LIBDIR}/qt5/plugins/sensors")
# get_target_property(PLUGIN_LOCATION Qt5::Sensors PLUGIN_LOCATION)

message(STATUS "Installing Qt5 sensors plugin to: ${PLUGIN_INSTALL_LOCATION}")

qt5_use_modules(qtubuntu_sensors_plugins Core Sensors)

target_link_libraries(
  qtubuntu_sensors_plugins 
  Qt5::Core 
  Qt5::Sensors

  ${UBUNTU_PLATFORM_API_LIBRARIES}
)

install(
  TARGETS qtubuntu_sensors_plugins
  LIBRARY DESTINATION ${PLUGIN_INSTALL_LOCATION})
