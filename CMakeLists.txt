cmake_minimum_required(VERSION 2.8)

project(qtubuntu-sensors)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

include(GNUInstallDirs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra -fPIC -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fno-strict-aliasing -fvisibility=hidden -fvisibility-inlines-hidden -pedantic -Wextra")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

option(QTUBUNTU_SENSORS_WERROR "Treat warnings as errors" ON)
if (QTUBUNTU_SENSORS_WERROR)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif(QTUBUNTU_SENSORS_WERROR)

# enable tests with gtest
include(CTest)
set(GTEST_ROOT /usr/src/gtest)
add_subdirectory(${GTEST_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/gtest)

find_package(Threads REQUIRED)

add_definitions(-DQT_PLUGIN)

add_subdirectory(plugins)
add_subdirectory(tests)
