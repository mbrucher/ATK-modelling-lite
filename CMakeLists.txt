cmake_minimum_required(VERSION 3.5)

project(Audio-modelling)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ENABLE_LOG "Enable modelling logging" OFF)
option(ENABLE_SHARED_LIBRARIES "Enable shared libraries generation" ON)
option(ENABLE_STATIC_LIBRARIES "Enable static libraries generation" OFF)

SET(CMAKE_MODULE_PATH ${ATK_DIR}/share/CMake)

find_package(ATK)
find_package(Boost REQUIRED log unit_test_framework)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_NO_AUTO_PTR)

include(Utilities)

add_subdirectory(ATK/Modelling)
add_subdirectory(Python/ATK/Modelling)
add_subdirectory(test)

FILE(GLOB_RECURSE CMAKE_OTHER_SRC Python/proto/*.py)

add_custom_target(Prototype SOURCES ${CMAKE_OTHER_SRC})
