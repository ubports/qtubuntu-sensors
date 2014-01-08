find_package(Qt5Core REQUIRED)
find_package(Qt5Feedback REQUIRED)

qt5_add_resources(FEEDBACK_RESOURCES feedback.qrc)

add_library(
  qtfeedback_ubuntu SHARED

  core_feedback.h
  core_feedback.cpp

  ${FEEDBACK_RESOURCES}
)

# Ideally, we would read the plugin installation location from cmake
# but this does not work currently.
set(PLUGIN_INSTALL_LOCATION "${CMAKE_INSTALL_LIBDIR}/qt5/feedback")
# get_target_property(PLUGIN_LOCATION Qt5::Feedback PLUGIN_LOCATION)

message(STATUS "Installing Qt5 feedback plugin to: ${PLUGIN_INSTALL_LOCATION}")

qt5_use_modules(qtfeedback_ubuntu Core Feedback)

target_link_libraries(qtfeedback_ubuntu Qt5::Core Qt5::Feedback)

install(
  TARGETS qtfeedback_ubuntu
  LIBRARY DESTINATION ${PLUGIN_INSTALL_LOCATION})


