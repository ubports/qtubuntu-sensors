find_package(Qt5Core REQUIRED)
find_package(Qt5Sensors REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROCESS_CPP process-cpp REQUIRED)

#include_directories(${GTEST_ROOT}/src)
include_directories(${PROCESS_CPP_INCLUDE_DIRS})

add_executable(
  test_sensor_api

  test_sensor_api.cpp
)

target_link_libraries(
  test_sensor_api
  
  gtest
  gtest_main
  ${PROCESS_CPP_LIBRARIES}
)

qt5_use_modules(
  test_sensor_api
  
  Core
  Sensors
)

add_executable(
  test_sensor_api_mock

  test_sensor_api_mock.cpp
)

target_link_libraries(
  test_sensor_api_mock
  
  gtest
  gtest_main
  ${PROCESS_CPP_LIBRARIES}
)

qt5_use_modules(
  test_sensor_api_mock
  
  Core
  Sensors
)

# we need to call the tests with pointing to the locally built plugins
add_test(
  test_sensor_api_mock
  
  # add QT_DEBUG_PLUGINS=1 if plugin isn't loading
  env QT_PLUGIN_PATH=${CMAKE_BINARY_DIR}/plugins ${CMAKE_CURRENT_BINARY_DIR}/test_sensor_api_mock
)

if(DEFINED ENV{UBUNTU_PLATFORM_API_BACKEND})
  add_test(
  test_sensor_api
  
    # add QT_DEBUG_PLUGINS=1 if plugin isn't loading
    env QT_PLUGIN_PATH=${CMAKE_BINARY_DIR}/plugins ${CMAKE_CURRENT_BINARY_DIR}/test_sensor_api
  )
endif()
